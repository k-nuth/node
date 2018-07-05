import copy
import os
import cpuid
import platform
from ci_utils import get_builder, handle_microarchs, copy_env_vars, filter_valid_exts

if __name__ == "__main__":

    full_build = os.getenv('BITPRIM_FULL_BUILD', '0') == '1'
    # full_build = True

    builder, name = get_builder()
    builder.add_common_builds(shared_option_name="%s:shared" % name)

    filtered_builds = []
    for settings, options, env_vars, build_requires, reference in builder.items:

        if settings["build_type"] == "Release" \
                and not options["%s:shared" % name]:

            copy_env_vars(env_vars)

            if os.getenv('BITPRIM_RUN_TESTS', 'false') == 'true':
                options["%s:with_tests" % name] = "True"
                marchs = ["x86-64"]
            else:
                if full_build:
                    # marchs = ["x86-64", ''.join(cpuid.cpu_microarchitecture()), "haswell", "skylake", "skylake-avx512"]
                    # marchs = [''.join(cpuid.cpu_microarchitecture()), 'znver1', 'silvermont', 'westmere', 'goldmont', 'btver1', 'icelake-client', 'btver2', 'skylake', 'nano', 'haswell', 'nano-1000', 'broadwell', 'bdver1', 'bdver3', 'bdver2', 'bdver4', 'core2', 'k8', 'amdfam10', 'icelake-server', 'bonnell', 'cannonlake', 'k8-sse3', 'goldmont-plus', 'nano-x4', 'nehalem', 'ivybridge', 'eden-x4', 'x86-64', 'nano-3000', 'knl', 'knm', 'penryn', 'eden-x2', 'sandybridge', 'nano-2000', 'tremont', 'skylake-avx512', 'nano-x2']
                    # marchs = marchs_compiler_list(str(platform.system()), str(settings["compiler"]), float(str(settings["compiler.version"])))
                    # marchs.append(''.join(cpuid.cpu_microarchitecture()))

                    marchs = filter_valid_exts(str(platform.system()), str(settings["compiler"]), float(str(settings["compiler.version"])), [''.join(cpuid.cpu_microarchitecture()), 'x86-64', 'sandybridge', 'ivybridge', 'haswell', 'skylake', 'skylake-avx512'])
                else:
                    marchs = ["x86-64"]

            opts_bch = copy.deepcopy(options)
            opts_btc = copy.deepcopy(options)
            # opts_ltc = copy.deepcopy(options)

            opts_bch["%s:currency" % name] = "BCH"
            opts_btc["%s:currency" % name] = "BTC"
            # opts_ltc["%s:currency" % name] = "LTC"

            handle_microarchs("%s:microarchitecture" % name, marchs, filtered_builds, settings, opts_bch, env_vars, build_requires)
            handle_microarchs("%s:microarchitecture" % name, marchs, filtered_builds, settings, opts_btc, env_vars, build_requires)
            # handle_microarchs("%s:microarchitecture" % name, marchs, filtered_builds, settings, opts_ltc, env_vars, build_requires)


    builder.builds = filtered_builds
    builder.run()
