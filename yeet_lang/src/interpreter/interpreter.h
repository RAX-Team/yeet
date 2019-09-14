#pragma once

#include <stdlib.h>

#include "data.h"
#include "stack.h"
#include "prelude.h"
#include "../parser/syntaxtree.h"
#include "io.h"

#define DEBUG false
#define DEBUGGING if(DEBUG)

Data* evaluate(Stack*, ASTNode*);
Data* execute(Stack*, ASTNode*);
Stack* run(Programme*);
Data* exec_function(T_Function*, unsigned int, Data**);