#ifndef MOENJIN_PARSER_PARSERMACROS_H
#define MOENJIN_PARSER_PARSERMACROS_H

#define Fail() do { if (!mHasError) SetErrorMessage(); return 0; } while (0)
#define FailWithToken(tok) do { if (!mHasError) SetErrorMessage(tok); return 0; } while (0)
#define FailWithMessage(msg) do { if (!mHasError) SetErrorMessage(msg); return 0; } while (0)
#define FailWithNameAndMessage(before, name, after) do { if (!mHasError) SetErrorMessageForNameWithMessage(name, before, after); return 0; } while (0)

#define FailIfNull(cond) do { if ((cond == mo_null)) Fail(); } while (0)
#define FailIfNullWithMessage(cond, msg) do { if ((cond == mo_null)) FailWithMessage(msg); } while (0)
#define FailIfNullWithNameAndMessage(cond, before, name, msg) do { if ((cond == mo_null)) FailWithNameAndMessage(before, name, msg); } while (0)

#define FailIfFalse(cond) do { if (!(cond)) Fail(); } while (0)
#define FailIfFalseWithMessage(cond, msg) do { if (!(cond)) FailWithMessage(msg); } while (0)
#define FailIfFalseWithNameAndMessage(cond, before, name, msg) do { if (!(cond)) FailWithNameAndMessage(before, name, msg); } while (0)

#define FailIfTrue(cond) do { if ((cond)) Fail(); } while (0)
#define FailIfTrueWithMessage(cond, msg) do { if ((cond)) FailWithMessage(msg); } while (0)
#define FailIfTrueWithNameAndMessage(cond, before, name, msg) do { if ((cond)) FailWithNameAndMessage(before, name, msg); } while (0)

#define FailIfTrueIfStrict(cond) do { if ((cond) && GetStrictMode()) Fail(); } while (0)
#define FailIfTrueIfStrictWithMessage(cond, msg) do { if ((cond) && GetStrictMode()) FailWithMessage(msg); } while (0)
#define FailIfTrueIfStrictWithNameAndMessage(cond, before, name, after) do { if ((cond) && GetStrictMode()) FailWithNameAndMessage(before, name, after); } while (0)

#define FailIfFalseIfStrict(cond) do { if ((!(cond)) && GetStrictMode()) Fail(); } while (0)
#define FailIfFalseIfStrictWithMessage(cond, msg) do { if ((!(cond)) && GetStrictMode()) FailWithMessage(msg); } while (0)
#define FailIfFalseIfStrictWithNameAndMessage(cond, before, name, after) do { if ((!(cond)) && GetStrictMode()) FailWithNameAndMessage(before, name, after); } while (0)

#define FailIfStackOverflow() do { FailIfFalseWithMessage(CanRecurse(), "Code nested too deeply."); } while (0)
#define FailIfError() FailIfTrue(mHasError)

#define ConsumeOrFail(tokenType) do { if (!Consume(tokenType)) FailWithToken(tokenType); } while (0)
#define MatchOrFail(tokenType) do { if (!Match(tokenType)) FailWithToken(tokenType); } while (0)

#endif