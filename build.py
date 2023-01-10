import copy
import os
import cpuid
import platform
from kthbuild import get_base_march_ids, get_builder, handle_microarchs, copy_env_vars, filter_marchs_tests

if __name__ == "__main__":
    full_build = os.getenv('KTH_FULL_BUILD', '0') == '1'
    builder, name = get_builder(os.path.dirname(os.path.abspath(__file__)))
    builder.add_common_builds(shared_option_name="%s:shared" % name)

    filtered_builds = []
    for settings, options, env_vars, build_requires, reference in builder.items:

        if settings["build_type"] == "Release" \
                and not options["%s:shared" % name]:

            copy_env_vars(env_vars)

            if os.getenv('KTH_RUN_TESTS', 'false') == 'true':
                options["%s:tests" % name] = "True"


            march_ids = get_base_march_ids()

            ci_currency = os.getenv('KTH_CI_CURRENCY', None)
            if ci_currency is None:
                opts_bch = copy.deepcopy(options)
                # opts_btc = copy.deepcopy(options)
                # opts_ltc = copy.deepcopy(options)

                opts_bch["%s:currency" % name] = "BCH"
                # opts_btc["%s:currency" % name] = "BTC"
                # opts_ltc["%s:currency" % name] = "LTC"

                # opts_btc_full = copy.deepcopy(opts_btc)
                # opts_btc_full["%s:db" % name] = "full"

                opts_bch_full = copy.deepcopy(opts_bch)
                opts_bch_full["%s:db" % name] = "full"

                handle_microarchs("%s:march_id" % name, march_ids, filtered_builds, settings, opts_bch_full, env_vars, build_requires)
                # handle_microarchs("%s:march_id" % name, march_ids, filtered_builds, settings, opts_btc_full, env_vars, build_requires)
                handle_microarchs("%s:march_id" % name, march_ids, filtered_builds, settings, opts_bch, env_vars, build_requires)
                # handle_microarchs("%s:march_id" % name, march_ids, filtered_builds, settings, opts_btc, env_vars, build_requires)
                # handle_microarchs("%s:march_id" % name, march_ids, filtered_builds, settings, opts_ltc, env_vars, build_requires)
            else:
                options["%s:currency" % name] = ci_currency
                # if ci_currency == "BCH":
                #     # opts_db_full = copy.deepcopy(options)
                #     # opts_db_full["%s:db" % name] = "full"

                opts_db_full = copy.deepcopy(options)
                opts_db_full["%s:db" % name] = "full"

                handle_microarchs("%s:march_id" % name, march_ids, filtered_builds, settings, opts_db_full, env_vars, build_requires)
                handle_microarchs("%s:march_id" % name, march_ids, filtered_builds, settings, options, env_vars, build_requires)


            filter_marchs_tests(name, filtered_builds, ["%s:tests" % name])


    builder.builds = filtered_builds
    builder.run()
