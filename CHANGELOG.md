# Changelog
All notable changes to this project will be documented in this file.

The format is based
on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/) and this
project adheres
to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [unreleased]
### Added
- ae_io for convenient reading and writing to/from file descriptors
- AE_STR_FROM_ARGS for populating a string from variable arguments

## [0.1.0] 21018-02-28
I should probably make this 1.0.0 since I changed ae_event to ae_mux
but I'm just going to bump the minor since I'd rather wait longer to
do a 1.0.0 release.
### Added
- ae_time_* functions
- AE_MEM_CLEAR macro for zeroing out a block of RAM
- CALLBACK_ONLY added to ae_opt option parser
- ae_pool_calloc for convenience
- ae_timer_t for periodic and single-shot functionality
### Fixed
- bug where substrings were being matched in option processing
- removed some debug output
### Changed
- ae_opt callbacks happen before the default action now
### Removed
- buf_t since it only makes life more difficult


## [0.0.2] - 2018-02-09
### Fixed
- fixed 64 bit macro test
- fixed ae-test to work with changes to ae_opt
## [0.0.1] - 2018-01-28
- initial release


## [Reference] - YYYY-MM-DD
-Added 
-Changed 
-Deprecated 
-Removed 
-Fixed 
-Security 
