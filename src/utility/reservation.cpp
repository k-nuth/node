// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bitcoin/node/utility/reservation.hpp>

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <boost/format.hpp>
#include <kth/domain.hpp>
#include <bitcoin/node/define.hpp>
#include <bitcoin/node/utility/performance.hpp>
#include <bitcoin/node/utility/reservations.hpp>

namespace libbitcoin {
namespace node {

using namespace std::chrono;
using namespace bc::chain;

// The allowed number of standard deviations below the norm.
// With 1 channel this multiple is irrelevant, no channels are dropped.
// With 2 channels a < 1.0 multiple will drop a channel on every test.
// With 2 channels a 1.0 multiple will fluctuate based on rounding deviations.
// With 2 channels a > 1.0 multiple will prevent all channel drops.
// With 3+ channels the multiple determines allowed deviation from the norm.
static constexpr float multiple = 1.01f;

// The minimum amount of block history to move the state from idle.
static constexpr size_t minimum_history = 3;

// Simple conversion factor, since we trace in micro and report in seconds.
static constexpr size_t micro_per_second = 1000 * 1000;

reservation::reservation(reservations& reservations, size_t slot,
    uint32_t sync_timeout_seconds)
  : rate_({ true, 0, 0, 0 }),
    stopped_(false),
    pending_(true),
    partitioned_(false),
    reservations_(reservations),
    slot_(slot),
    rate_window_(minimum_history * sync_timeout_seconds * micro_per_second)
{
}

reservation::~reservation()
{
    // This complicates unit testing and isn't strictly necessary.
    ////KTH_ASSERT_MSG(heights_.empty(), "The reservation is not empty.");
}

size_t reservation::slot() const
{
    return slot_;
}

bool reservation::pending() const
{
    return pending_;
}

void reservation::set_pending(bool value)
{
    pending_ = value;
}

std::chrono::microseconds reservation::rate_window() const
{
    return rate_window_;
}

high_resolution_clock::time_point reservation::now() const
{
    return high_resolution_clock::now();
}

// Rate methods.
//-----------------------------------------------------------------------------

// Clears rate/history but leaves hashes unchanged.
void reservation::reset()
{
    set_rate({ true, 0, 0, 0 });
    clear_history();
}

// Shortcut for rate().idle call.
bool reservation::idle() const
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    shared_lock lock(rate_mutex_);

    return rate_.idle;
    ///////////////////////////////////////////////////////////////////////////
}

void reservation::set_rate(performance&& rate)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    unique_lock lock(rate_mutex_);

    rate_ = std::move(rate);
    ///////////////////////////////////////////////////////////////////////////
}

// Get a copy of the current rate.
performance reservation::rate() const
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    shared_lock lock(rate_mutex_);

    return rate_;
    ///////////////////////////////////////////////////////////////////////////
}

// Ignore idleness here, called only from an active channel, avoiding a race.
bool reservation::expired() const
{
    auto const record = rate();
    auto const normal_rate = record.normal();
    auto const statistics = reservations_.rates();
    auto const deviation = normal_rate - statistics.arithmentic_mean;
    auto const absolute_deviation = std::fabs(deviation);
    auto const allowed_deviation = multiple * statistics.standard_deviation;
    auto const outlier = absolute_deviation > allowed_deviation;
    auto const below_average = deviation < 0;
    auto const expired = below_average && outlier;

    ////LOG_DEBUG(LOG_NODE)
    ////    << "Statistics for slot (" << slot() << ")"
    ////    << " adj:" << (normal_rate * micro_per_second)
    ////    << " avg:" << (statistics.arithmentic_mean * micro_per_second)
    ////    << " dev:" << (deviation * micro_per_second)
    ////    << " sdv:" << (statistics.standard_deviation * micro_per_second)
    ////    << " cnt:" << (statistics.active_count)
    ////    << " neg:" << (below_average ? "T" : "F")
    ////    << " out:" << (outlier ? "T" : "F")
    ////    << " exp:" << (expired ? "T" : "F");

    return expired;
}

void reservation::clear_history()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    unique_lock lock(history_mutex_);

    history_.clear();
    ///////////////////////////////////////////////////////////////////////////
}

// It is possible to get a rate update after idling and before starting anew.
// This can reduce the average during startup of the new channel until start.
void reservation::update_rate(size_t events, const microseconds& database)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    history_mutex_.lock();

    performance rate{ false, 0, 0, 0 };
    auto const end = now();
    auto const event_start = end - microseconds(database);
    auto const start = end - rate_window();
    auto const history_count = history_.size();

    // Remove expired entries from the head of the queue.
    for (auto it = history_.begin(); it != history_.end() && it->time < start;
        it = history_.erase(it));

    auto const window_full = history_count > history_.size();
    auto const event_cost = static_cast<uint64_t>(database.count());
    history_.push_back({ events, event_cost, event_start });

    // We can't set the rate until we have a period (two or more data points).
    if (history_.size() < minimum_history)
    {
        history_mutex_.unlock();
        //---------------------------------------------------------------------
        return;
    }

    // Summarize event count and database cost.
    for (auto const& record: history_)
    {
        KTH_ASSERT(rate.events <= max_size_t - record.events);
        rate.events += record.events;
        KTH_ASSERT(rate.database <= max_uint64 - record.database);
        rate.database += record.database;
    }

    // Calculate the duration of the rate window.
    auto window = window_full ? rate_window() : (end - history_.front().time);
    auto count = duration_cast<microseconds>(window).count();
    rate.window = static_cast<uint64_t>(count);

    history_mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    ////LOG_DEBUG(LOG_NODE)
    ////    << "Records (" << slot() << ") "
    ////    << " size: " << rate.events
    ////    << " time: " << divide<double>(rate.window, micro_per_second)
    ////    << " cost: " << divide<double>(rate.database, micro_per_second)
    ////    << " full: " << (full ? "true" : "false");

    // Update the rate cache.
    set_rate(std::move(rate));
}

// Hash methods.
//-----------------------------------------------------------------------------

bool reservation::empty() const
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    shared_lock lock(hash_mutex_);

    return heights_.empty();
    ///////////////////////////////////////////////////////////////////////////
}

size_t reservation::size() const
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    shared_lock lock(hash_mutex_);

    return heights_.size();
    ///////////////////////////////////////////////////////////////////////////
}

bool reservation::stopped() const
{
    // Critical Section (stop)
    ///////////////////////////////////////////////////////////////////////////
    shared_lock lock(stop_mutex_);

    return stopped_;
    ///////////////////////////////////////////////////////////////////////////
}

// Obtain and clear the outstanding blocks request.
message::get_data reservation::request(bool new_channel)
{
    message::get_data packet;

    // We are a new channel, clear history and rate data, next block starts.
    if (new_channel)
        reset();

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    hash_mutex_.lock_upgrade();

    if (!new_channel && !pending_)
    {
        hash_mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return packet;
    }

    // Build get_blocks request message.
    for (auto height = heights_.right.begin(); height != heights_.right.end();
        ++height)
    {
        static auto const id = message::inventory::type_id::block;
        packet.inventories().emplace_back(id, height->second);
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    hash_mutex_.unlock_upgrade_and_lock();
    pending_ = false;
    hash_mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    return packet;
}

void reservation::insert(hash_digest&& hash, size_t height)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    unique_lock lock(hash_mutex_);

    pending_ = true;
    heights_.insert({ std::move(hash), height });
    ///////////////////////////////////////////////////////////////////////////
}

void reservation::import(block_const_ptr block)
{
    size_t height;
    auto const hash = block->header().hash();
    auto const encoded = encode_hash(hash);

    if (!find_height_and_erase(hash, height))
    {
        LOG_DEBUG(LOG_NODE)
            << "Ignoring unsolicited block (" << slot() << ") ["
            << encoded << "]";
        return;
    }

    bool success;
    auto const importer = [this, &block, &height, &success]()
    {
        success = reservations_.import(block, height);
    };

    // Do the block import with timer.
    auto const cost = timer<microseconds>::duration(importer);

    if (success)
    {
        static auto const unit_size = 1u;
        update_rate(unit_size, cost);
        auto const record = rate();
        static auto const formatter =
            "Imported block #%06i (%02i) [%s] %06.2f %05.2f%%";

        LOG_INFO(LOG_NODE)
            << boost::format(formatter) % height % slot() % encoded %
            (record.total() * micro_per_second) % (record.ratio() * 100);
    }
    else
    {
        // This could also result from a block import failure resulting from
        // inserting at a height that is already populated, but that should be
        // precluded by the implementation. This is the only other failure.
        LOG_DEBUG(LOG_NODE)
            << "Stopped before importing block (" << slot() << ") ["
            << encoded << "]";
    }

    populate();
}

void reservation::populate()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    stop_mutex_.lock_upgrade();

    if (!stopped_ && empty())
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        stop_mutex_.unlock_upgrade_and_lock();
        stopped_ = !reservations_.populate(shared_from_this());
        stop_mutex_.unlock();
        //---------------------------------------------------------------------
        return;
    }

    stop_mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////
}

bool reservation::toggle_partitioned()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    hash_mutex_.lock_upgrade();

    if (partitioned_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        hash_mutex_.unlock_upgrade_and_lock();
        partitioned_ = false;
        pending_ = true;
        hash_mutex_.unlock();
        //---------------------------------------------------------------------
        return true;
    }

    hash_mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    return false;
}

// Give the minimal row ~ half of our hashes, return false if minimal is empty.
bool reservation::partition(reservation::ptr minimal)
{
    // This assumes that partition has been called under a table mutex.
    if (!minimal->empty())
        return true;

    // Critical Section (hash)
    ///////////////////////////////////////////////////////////////////////////
    hash_mutex_.lock_upgrade();

    // This addition is safe.
    // Take half of the maximal reservation, rounding up to get last entry.
    auto const offset = (heights_.size() + 1u) / 2u;
    auto it = heights_.right.begin();

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    hash_mutex_.unlock_upgrade_and_lock();

    // TODO: move the range in a single command.
    for (size_t index = 0; index < offset; ++index)
    {
        minimal->heights_.right.insert(std::move(*it));
        it = heights_.right.erase(it);
    }

    partitioned_ = !heights_.empty();
    auto const populated = !minimal->empty();
    minimal->pending_ = populated;

    if (!partitioned_)
    {
        // Critical Section (stop)
        ///////////////////////////////////////////////////////////////////////
        unique_lock lock(stop_mutex_);

        // If the channel has been emptied it will not repopulate.
        stopped_ = true;
        ///////////////////////////////////////////////////////////////////////
    }

    hash_mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    if (populated)
        LOG_DEBUG(LOG_NODE)
            << "Moved [" << minimal->size() << "] blocks from slot (" << slot()
            << ") to (" << minimal->slot() << ") leaving [" << size() << "].";

    return populated;
}

bool reservation::find_height_and_erase(const hash_digest& hash,
    size_t& out_height)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    hash_mutex_.lock_upgrade();

    auto const it = heights_.left.find(hash);

    if (it == heights_.left.end())
    {
        hash_mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return false;
    }

    out_height = it->second;

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    hash_mutex_.unlock_upgrade_and_lock();
    heights_.left.erase(it);
    hash_mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    return true;
}

} // namespace node
} // namespace kth
