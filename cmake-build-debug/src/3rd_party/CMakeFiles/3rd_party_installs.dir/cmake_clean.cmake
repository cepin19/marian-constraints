file(REMOVE_RECURSE
  "../../local"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/3rd_party_installs.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
