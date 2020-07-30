#pragma once

#include <memory>
#include <string>

std::shared_ptr<vasSAT::CNFFormula> parseCNFFile(const std::string &path);