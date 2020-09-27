#include "ExpressionManager.h"

const string ExpressionManager::OPERATORS = "+-*/%";
const string ExpressionManager::OPEN = "([{";
const string ExpressionManager::CLOSED = ")]}";

bool ExpressionManager::isBalanced(string expression){
	cout << "isBalanced: " << expression << endl;

	stack<char> s;
	bool balanced = true;
    for (string::const_iterator iter = expression.begin();
     balanced && (iter != expression.end());
     iter++) {
        char next_ch = *iter;
        if (OPEN.find(next_ch) != string::npos) {
            s.push(next_ch);
        } else if (CLOSED.find(next_ch) != string::npos) {
            if (s.empty()) {
                balanced = false;
            } else {
                char top_ch = s.top();
                s.pop();
                balanced = 
                 OPEN.find(top_ch) == CLOSED.find(next_ch);
            }
        }
    }
    return balanced && s.empty();
}

string ExpressionManager::postfixToInfix(string postfixExpression){
	cout << "postfixToInfix: " << postfixExpression << endl;


	stack <string> infixStack;
	string lhs;
	string rhs;

	istringstream tokens(postfixExpression);
  	char next_char;
	  
  	while (tokens >> next_char) {
		if (isdigit(next_char)) { //token is a int
			tokens.putback(next_char);
			string value;
			tokens >> value;
			if (value.find(".") != string::npos) { //the int is a double
				return "invalid";
			}
			infixStack.push(value);
		}
		else if (is_operator(next_char) || is_bracket(next_char)) { //token is an operator
			if (infixStack.size() < 2) {
				return "invalid";
			}
			else {
				rhs = infixStack.top();
				infixStack.pop();
				lhs = infixStack.top();
				infixStack.pop();

				infixStack.push("( " + lhs + " " + next_char + " " + rhs + " )");
			}
		}
		else { //invalid token
			return "invalid";
		}
	}

	if (infixStack.size() > 1) { //more than one value left in stack
		return "invalid";
	}
	return infixStack.top();
}

string ExpressionManager::postfixEvaluate(string postfixExpression){
	cout << "In postfixEvaluate "<< postfixExpression<<endl;
// Be sure the stack is empty
  while (!operand_stack.empty())
    operand_stack.pop();

  // Process each token
  istringstream tokens(postfixExpression);
  char next_char;
  while (tokens >> next_char) {
    if (isdigit(next_char)) {
      tokens.putback(next_char);
      int value;
      tokens >> value;
      operand_stack.push(value);
    } else if (is_operator(next_char)) {
      int result;
      try {
        result = eval_op(next_char);
      }
      catch(int which) {
          cout << "not enough operands" << endl;
          return("invalid");
      }
      operand_stack.push(result);
    } else {
      cout << "not digit or operand";
      return("invalid");
    }
  }
  if (!operand_stack.empty()) {
    string answer = to_string(operand_stack.top());
    operand_stack.pop();
    if (operand_stack.empty()) {
      return answer;
    } else {
      cout << "at end stack not empty";
      return("invalid");
    }
  } else {
    cout << "at end answer not on stack";
    return("invalid");
  }
}

/** Evaluates the current operator.
    This function pops the two operands off the operand
    stack and applies the operator.
    @param op A character representing the operator
    @throws Syntax_Error if top is attempted on an empty stack
*/
int ExpressionManager::eval_op(char op) {
  if (operand_stack.empty()) 
    throw 1;
  int rhs = operand_stack.top();
  operand_stack.pop();
  if (operand_stack.empty())
    throw 2;
  int lhs = operand_stack.top();
  operand_stack.pop();
  int result = 0;
  switch(op) {
  case '+' : result = lhs + rhs;
             break;
  case '-' : result = lhs - rhs;
             break;
  case '*' : result = lhs * rhs;
             break;

  case '/' : if (rhs == 0) { // dividing by 0
	  			 throw 3;
  			 }
  			 result = lhs / rhs;
             break;
  case '%' : if (rhs == 0) { // dividing by 0
	  			 throw 3;
  			 }
			 result = lhs % rhs;
             break;
  }
  if (!isfinite(result)) 
  	throw 3;
  return result;
}

string ExpressionManager::infixToPostfix(string infixExpression){
	cout << "infixToPostfix: " << infixExpression << endl;

	stack<char> operators;
	string postfixString = "";
	istringstream tokens(infixExpression);
  	char next_char;

	if (!validInfix(infixExpression)) {
		return "invalid";
	}

  	while (tokens >> next_char) {
		if (isdigit(next_char)) { //token is a int
		tokens.putback(next_char);
		string value;
		tokens >> value;
		if (value.find(".") != string::npos) { //the int is a double
			return "invalid";
		}
		postfixString.append(value + " ");
		} 
		else if (is_operator(next_char) || is_bracket(next_char)) { // token is an operator or bracket
			if (operators.empty()) {
				operators.push(next_char);
			}
			else {
				if (get_precedence(next_char) == 0) { //wild precedence
					operators.push(next_char);
				}
				else if (get_precedence(next_char) == 3) { //immediate precedence
					while (CLOSED.find(next_char) != OPEN.find(operators.top())) {
						postfixString.push_back(operators.top());
						postfixString.push_back(' ');
						operators.pop();
					}
					operators.pop();
				}
				else if (get_precedence(next_char) > get_precedence(operators.top())) { // precedence is greater than stack top
					operators.push(next_char);
				}
				else {
					while(!operators.empty() && get_precedence(next_char) <= get_precedence(operators.top())) { // precedence is less than or equal to stack top
						postfixString.push_back(operators.top());
						postfixString.push_back(' ');
						operators.pop();

					}
					operators.push(next_char);
				}
			}
		}
		else { //invalid token
			return "invalid";
		}
	}

	while (!operators.empty()) {
		postfixString.push_back(operators.top());
		postfixString.push_back(' ');
		operators.pop();
	}
	postfixString.pop_back();

	return postfixString;
}

int ExpressionManager::get_precedence(char ch) const {
	string immediate = ")]}";
	string high = "*/%";
	string low = "+-";
	string wild = "([{";

	if (immediate.find(ch) != string::npos) {
		return 3;
	}
	else if (high.find(ch) != string::npos) {
		return 2;
	}
	else if (low.find(ch) != string::npos) {
		return 1;
	}
	else if (wild.find(ch) != string::npos) {
		return 0;
	}
	else {
		return -1;
	}
}

bool ExpressionManager::validInfix(string infixExpression) {
	if (!isBalanced(infixExpression)) {
		return false;
	}

	istringstream tokens(infixExpression);
  	char next_char;
	stack<char> checkStack;
	bool checkedFirst = false;

  	while (tokens >> next_char) {
		if(!checkedFirst && (is_operator(next_char) || CLOSED.find(next_char) != string::npos)) {//first token is operator or closed bracket
			return false;
		}
		else {
			checkedFirst = true;
		}

		if (!checkStack.empty()) {
			if (is_operator(next_char) && is_operator(checkStack.top())) { //double operator
			  return false;
		  	}
			else if (CLOSED.find(next_char) != string::npos && is_operator((checkStack.top()))) { //operator before close bracket
			  return false;
			}
			else if (OPEN.find(checkStack.top()) != string::npos && is_operator((next_char))) { //operator after open bracket
			  return false;
			}
		}
		checkStack.push(next_char);
	}
	return true;
}
