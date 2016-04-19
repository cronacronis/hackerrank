/*
 * Your task is to take these partial results and make them "nice" by removing all unnecessary parentheses.
 Input

 There is a single positive integer T on the first line of input (equal to about 10000).
 It stands for the number of expressions to follow.
 Each expression consists of a single line containing only
 lowercase letters, operators (+, -, *, /) and parentheses (( and )).

 The letters are variables that can have any value, operators and parentheses have their usual meaning.
 Multiplication and division have higher priority then subtraction and addition.
 All operations with the same priority are computed from left to right (operators are left-associative).
 There are no spaces inside the expressions. No input line contains more than 250 characters.
 *
 * */

#include <iterator>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::shared_ptr;
using std::make_shared;


enum class ExprType {
	Container,
	Brackets,
	Constant,
	Operator,
	Empty
};


/*
 * For example, subtraction and division, as used in conventional math notation, are inherently left-associative.
 * Addition and multiplication, by contrast, are both left and right associative.
 * */
enum class OpType {
	NA = 0,
	Sum = 1,
	Minus = 2,
	Product = 3,
	Div = 4
};

std::ostream& operator << (std::ostream& os, const ExprType& obj)
{
   os << static_cast<std::underlying_type<ExprType>::type>(obj);
   return(os);
}

std::ostream& operator << (std::ostream& os, const OpType& obj)
{
   os << static_cast<std::underlying_type<OpType>::type>(obj);
   return(os);
}

void wrapin_brackets(string& str)
{
	str += ")";
	str.insert (0, 1, '(');
}

/* returns string with evertything between matching brackets with brackets included */
string get_between_brackets(string::iterator& tail_, const string::iterator& end_)
{
	string brackets_content;
	if( (end_ - tail_) >= 2)
	{
		uint br_cnt = 0;
		while(tail_ != end_)
		{
			const auto& chr = *tail_;
			switch(chr)
			{
				case '(': br_cnt++; break;
				case ')': br_cnt--; break;
			}
			if(br_cnt >= 0)
				brackets_content.push_back(chr);

			if (br_cnt == 0)
			{
				break;
			}
			++tail_;
		}
	} else throw std::invalid_argument("brackets tail is not long enough");
	return(brackets_content);
}

struct ExprContainer
{
	ExprType type;
	string str;
	vector<shared_ptr<ExprContainer>> children;

	/* This variable wraps children in brackets*/
	bool extr_brackets = false;

	OpType operator_type = OpType::NA;

	bool isset_operator_inside = false;
	OpType operator_inside = operator_type;


	ExprContainer(ExprType type_, const string str_) : type(type_), str(str_) {
		if(type_ == ExprType::Operator)
		{
			switch(this->str.front())
			{
				case '+': this->operator_type = OpType::Sum; break;
				case '-': this->operator_type =  OpType::Minus; break;
				case '*': this->operator_type =  OpType::Product; break;
				case '/': this->operator_type =  OpType::Div; break;
				throw std::invalid_argument("input is not an operator");
			}
		}
	};

	ExprContainer(string str = "") : ExprContainer(ExprType::Container, str) {};

	void addChild(shared_ptr<ExprContainer>& chld_ptr)
	{
		children.emplace_back(chld_ptr);
	}
	shared_ptr<ExprContainer> addChild(ExprType type_, string str_)
	{
		shared_ptr<ExprContainer> chld = make_shared<ExprContainer>(type_, str_);
		children.emplace_back(chld);
		return(chld);
	}

	void addChild(ExprType type_, const char& chr_)
	{
		addChild(type_, string(1, chr_));
	}

	void setBrackets()
	{
		if(this->extr_brackets == false)
			this->extr_brackets = true;
	}

	void removeBrackets()
	{
		if(this->type == ExprType::Brackets)
		{
			str.pop_back();
			str.erase(str.begin());
		} else {
			throw std::logic_error("Brackets can be only removed for brackets container");
		}
	}

	bool hasChildren()
	{
		return(!children.empty());
	}

	/* return operation inside the container or operation of itself*/
	OpType opTypeInside()
	{
		if(isset_operator_inside)
		{
			return(this->operator_inside);
		} else if(hasChildren()) {
			OpType max_op_type = this->operator_type;
			for(auto& chld : children)
			{
				ExprContainer& chldref = (*chld);
				auto op_type = chldref.opTypeInside();
				if(op_type > max_op_type) max_op_type = op_type;
			}
			this->operator_inside = max_op_type;
			this->isset_operator_inside = true;
			return(max_op_type);
		} else {
			return(this->operator_type);
		}
	}

	string extract()
	{
		string extract;
		if(!hasChildren())
		{
			return(str);
		}
		else{
			bool with_brackets = this->extr_brackets;
			if(with_brackets) extract += '(';
			for (const auto& p : children)
			{
				extract += p->extract();
			}
			if(with_brackets) extract += ')';
		}
		return(extract);
	}


};
//////////////////////////////////////////////////
inline bool is_bracket(const char& chr)
{
	return (chr == '(' || chr == ')');
}

inline bool is_operator(const char& chr)
{
	return (chr == '*' || chr == '+' || chr == '/' || chr == '-');
}

void parse_string(ExprContainer& cont_)
{
	string::iterator start = cont_.str.begin();
	const string::iterator& end = cont_.str.end();
	for(auto& it = start; it != end; ++it)
	{
		const auto& cur_chr = *it;
		if(isspace(cur_chr))
		{
			continue;
		} else if(isalpha(cur_chr))
		{
			cont_.addChild(ExprType::Constant, cur_chr);
		} else if (is_operator(cur_chr))
		{
			cont_.addChild(ExprType::Operator, cur_chr);
		} else if (is_bracket(cur_chr))
		{
			auto txt_in_brackets = get_between_brackets(it, end);
			shared_ptr<ExprContainer> bracketsCont = make_shared<ExprContainer>(ExprType::Brackets, txt_in_brackets);
			bracketsCont->removeBrackets();
			parse_string(*bracketsCont);
			if(!bracketsCont->hasChildren())
			{
				cont_.addChild(ExprType::Empty, "");
			}
			else
			{
				cont_.addChild(bracketsCont);
			}
		}
	}
	cont_.str.erase(cont_.str.begin(), cont_.str.end());
}
//////////////////////////////////////////////////
/*                       +  -  *  /    */
int left_prios[5]  = {0, 3, 3, 4, 7};
int opr_prios[5]   = {0, 2, 3, 4, 7};
int right_prios[5] = {0, 2, 2, 4, 5};

inline int opr_prio(OpType operat)
{
	return(opr_prios[(int)operat]);
}
inline int left_prio(OpType operand)
{
	return(left_prios[(int)operand]);
}
inline int right_prio(OpType operand)
{
	return(right_prios[(int)operand]);
}

std::tuple<bool, bool>
which_to_wrap(OpType opr, OpType left, OpType right)
{
	bool towrapleft = false;
	bool towrapright = false;
	int opr_pr = opr_prio(opr);
	if(opr_pr > left_prio(left)) {
		towrapleft = true;
	}
	if(opr_pr > right_prio(right)) {
		towrapright = true;
	}
	return(std::make_tuple(towrapleft, towrapright));
}

void mark_brackets(ExprContainer& input_cont_)
{
	for(auto opr_it = input_cont_.children.begin(); opr_it !=  input_cont_.children.end(); ++opr_it)
	{
		ExprContainer& curr_opr = (**opr_it);
		if(curr_opr.hasChildren())
		{
			mark_brackets(curr_opr);
		} else if (curr_opr.type == ExprType::Operator) {
			auto& prev_chld = **(opr_it-1);
			auto& next_chld = **(opr_it+1);
			OpType left = prev_chld.opTypeInside();
			OpType right = next_chld.opTypeInside();
			bool left_brackets = false;
			bool right_brackets = false;
			std::tie(left_brackets, right_brackets) = which_to_wrap(curr_opr.opTypeInside(), left, right);
			if(left_brackets == true) prev_chld.setBrackets();
			if(right_brackets == true) next_chld.setBrackets();
		}
	}
}

string remove_excess_brackets(string str)
{
	ExprContainer top(str);
	parse_string(top);
	mark_brackets(top);
	string extract = top.extract();
	return(extract);
}

int main()
{
	int num_of_expr;
	std::cin >> num_of_expr;
	vector<string> unparsed(num_of_expr);
	for(int i = 0; i < num_of_expr; i++)
	{
		std::cin >> unparsed[i];

	}
	for(auto& unpr : unparsed)
		cout << remove_excess_brackets(unpr) << endl;

	return(0);
}

