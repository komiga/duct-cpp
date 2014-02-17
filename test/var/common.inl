
void
tabulate(
	signed count
) {
	while (0 < count--) {
		std::cout << "  ";
	}
}

void
print_var(
	duct::Variable const& var,
	signed tab_count = 0,
	bool const newline = true
) {
	tabulate(tab_count);
	std::cout
		<< '(' << std::left << std::setw(10)
		<< duct::detail::get_vartype_name(var.get_type())
		<< ") '"
		<< var.get_name()
		<< "' = "
	;
	switch (var.get_type()) {
	case duct::VarType::array:
	case duct::VarType::node:
	case duct::VarType::identifier: {
		std::cout << '(' << var.size() << ") [";
		if (0u != var.size()) {
			++tab_count;
			std::cout << '\n';
			for (
				auto it = var.cbegin();
				var.cend() != it;
				++it
			) {
				print_var(*it, tab_count, false);
				if (1 < var.cend() - it) {
					std::cout << ",\n";
					//tabulate(tab_count);
				}
			}
			--tab_count;
			std::cout << '\n';
			tabulate(tab_count);
		}
		std::cout << ']';
		break;
	}
	case duct::VarType::string:
		std::cout << '"' << var.get_string_ref() << '"'; break;
	default:
		std::cout << var.get_as_str(); break;
	}
	if (newline) {
		std::cout << '\n';
	}
}
