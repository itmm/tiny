#pragma once

#include <string>
#include <memory>

class Declaration {
		const std::string name_;
	protected:
		Declaration(std::string name): name_ { name } { }

	public:
		using Ptr = std::shared_ptr<Declaration>;
		virtual ~Declaration() { }

		const std::string &name() const { return name_; }
};
