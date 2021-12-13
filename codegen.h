#ifndef JAMCC_CODEGEN_H
#define JAMCC_CODEGEN_H

#include "parser.h"

void generate_expression(struct Expression* node);
void generate_statement(struct Statement* statement);
void generate_program(struct Program* program);

#endif
