#ifndef MOENJIN_AST_CODEFEATURES_H
#define MOENJIN_AST_CODEFEATURES_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	typedef mo_unsigned CodeFeatures;

    const CodeFeatures CodeFeaturesNone = 0;
    const CodeFeatures CodeFeaturesEval = 1 << 0;
    const CodeFeatures CodeFeaturesArguments = 1 << 1;
    const CodeFeatures CodeFeaturesWith = 1 << 2;
    const CodeFeatures CodeFeaturesCatch = 1 << 3;
    const CodeFeatures CodeFeaturesThis = 1 << 4;
    const CodeFeatures CodeFeaturesStrictMode = 1 << 5;
    const CodeFeatures CodeFeaturesShadowsArguments = 1 << 6;
    const CodeFeatures CodeFeaturesAll = CodeFeaturesEval | CodeFeaturesArguments | CodeFeaturesWith | CodeFeaturesCatch | CodeFeaturesThis | CodeFeaturesStrictMode | CodeFeaturesShadowsArguments;
}

#endif