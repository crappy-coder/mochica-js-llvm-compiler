#ifndef MOENJIN_RUNTIME_JSCLASSINFO_H
#define MOENJIN_RUNTIME_JSCLASSINFO_H

#include "Platform/Configuration.h"
#include "Runtime/JSClassFunctionTable.h"
#include "Runtime/JSClassPropertyTable.h"

namespace MoEnjin
{
	#define JS_CLASS() static const JSClassInfo ClassInfo

	#define JS_DEFINE_AS_EMPTY_CLASS(cls, name) \
		const JSClassInfo cls##::ClassInfo = { #name, 0, 0, CREATE_EMPTY_CLASS_FUNCTION_TABLE(cls) }

	// define object as a base class that has a property table
	#define JS_DEFINE_AS_BASE_CLASS(cls, name) \
		const JSClassInfo cls##::ClassInfo = { #name, 0, &cls##Table, CREATE_CLASS_FUNCTION_TABLE(cls) }

	// define object as a base class that does not have a property table (i.e. naked)
	#define JS_DEFINE_AS_NAKED_BASE_CLASS(cls, name) \
		const JSClassInfo cls##::ClassInfo = { #name, 0, 0, CREATE_CLASS_FUNCTION_TABLE(cls) }

	// define object as a subclass that has a property table
	#define JS_DEFINE_AS_SUBCLASS(cls, name, parent) \
		const JSClassInfo cls##::ClassInfo = { #name, &parent##::ClassInfo, &cls##Table, CREATE_CLASS_FUNCTION_TABLE(cls) }

	// define object as a subclass that does not have a property table (i.e. naked)
	#define JS_DEFINE_AS_NAKED_SUBCLASS(cls, name, parent) \
		const JSClassInfo cls##::ClassInfo = { #name, &parent##::ClassInfo, 0, CREATE_CLASS_FUNCTION_TABLE(cls) }

	struct JSClassInfo
	{
		const mo_utf8stringptr Name;
		const JSClassInfo* Parent;
		const JSClassPropertyTable* Properties;
		JSClassFunctionTable Functions;

		mo_bool HasStaticProperties() const
		{
			for(const JSClassInfo* c = this; c; c = c->Parent)
			{
				if(c->Properties)
					return mo_true;
			}

			return mo_false;
		}
	};

	mo_inline mo_bool jsIsSubClass(const JSClassInfo* derived, const JSClassInfo* parent)
	{
		for(const JSClassInfo* c = derived; c; c = c->Parent)
		{
			if(c == parent)
				return mo_true;
		}

		return mo_false;
	}
}

#endif