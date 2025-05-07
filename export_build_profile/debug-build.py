# Use this to compile the editor to be usable in another debugging standalone programs.

# See https://docs.godotengine.org/en/stable/contributing/development/compiling/optimizing_for_size.html for optimize flag lists.
# See https://docs.godotengine.org/en/stable/contributing/development/compiling/introduction_to_the_buildsystem.html#overriding-the-build-options for more info on how to use this file.

production = "yes"
debug_symbols = "yes"
optimize = "debug"

use_asan = "yes"
use_lsan = "yes"
use_msan = "yes"