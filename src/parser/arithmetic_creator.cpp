//
// Created by shrum on 07.05.18.
//

#include "arithmetic_creator.h"
#include "parser_wrapper.h"

class ArithmeticCreator : public AstVisitor{
public:
  explicit ArithmeticCreator(const NameResolver& resolver)
      : resolver_(resolver)
  {
  }

  std::unique_ptr<ArithmeticOperation> ExtractOperation()
  {
    return std::move(operation_);
  }
private:
  void dispatch(const rbg_parser::integer_arithmetic &arithmetic) override;
  void dispatch(const rbg_parser::variable_arithmetic &arithmetic) override;
  void dispatch(const rbg_parser::arithmetic_operation &arithmetic) override;
  const NameResolver& resolver_;

  std::unique_ptr<ArithmeticOperation> operation_;
};

std::unique_ptr<ArithmeticOperation>
CreateOperation(const rbg_parser::arithmetic_expression &expression, const NameResolver &resolver) {
  ArithmeticCreator creator(resolver);
  expression.accept(creator);
  return creator.ExtractOperation();
}

void ArithmeticCreator::dispatch(const rbg_parser::integer_arithmetic &arithmetic) {
  operation_ = std::unique_ptr<ArithmeticOperation>(new arithmetic_operations::Constant(arithmetic.get_content()));
}

void ArithmeticCreator::dispatch(const rbg_parser::variable_arithmetic &arithmetic) {
  token_id_t variable_id = resolver_.Id(arithmetic.get_content().to_string());
  operation_ = std::unique_ptr<ArithmeticOperation>(new arithmetic_operations::Variable(variable_id));
}

void ArithmeticCreator::dispatch(const rbg_parser::arithmetic_operation &arithmetic) {
  std::vector<std::unique_ptr<ArithmeticOperation> > operations;
  for (const auto &child : arithmetic.get_content()) {
    operations.push_back(CreateOperation(*child, resolver_));
  }
  switch(arithmetic.get_operation())
  {
    case rbg_parser::addition:
      operation_ = std::unique_ptr<ArithmeticOperation>(new arithmetic_operations::Sum(std::move(operations)));
      break;
    case rbg_parser::subtraction:
      operation_ = std::unique_ptr<ArithmeticOperation>(new arithmetic_operations::Subtraction(std::move(operations)));
      break;
    case rbg_parser::multiplication:
      operation_ = std::unique_ptr<ArithmeticOperation>(new arithmetic_operations::Product(std::move(operations)));
      break;
    case rbg_parser::division:
      operation_ = std::unique_ptr<ArithmeticOperation>(new arithmetic_operations::Division(std::move(operations)));
      break;
  }
}
