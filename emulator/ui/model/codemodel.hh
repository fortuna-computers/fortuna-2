#ifndef EMULATOR_CODEMODEL_HH
#define EMULATOR_CODEMODEL_HH

#include "../../compiler/debug.hh"

class CodeModel {
public:
    explicit CodeModel(Debug const& debug) : debug_(debug) {}

private:
    Debug const& debug_;
};

#endif
