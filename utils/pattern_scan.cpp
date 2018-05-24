#include "pattern_scan.h"
#include "memutils.h"
#include "string.h"
#include <vector>
#include "assert.h"
#include "stdlib.h"

struct pOperation
{
	short op;
	intptr_t offset, v1;

	pOperation(short o = 0, intptr_t off = 0, intptr_t v = 0)
	{
		op = o;
		offset = off;
		v1 = v;
	}

	uintptr_t RunOp(uintptr_t addr)
	{
		switch(op) {
		  case 0:
			  return *(uintptr_t*)(addr + offset);
		  case 1:
			  return addr + v1;
		  case 2:
			  return GetAbsoluteAddress(addr, offset, v1);
		}
		return addr;
	}
};

static void ParsePattern(const char* pattern, short*& patternBytes, size_t& length, std::vector<pOperation>& operations)
{
	char* p = (char*)pattern-1;
	bool inRelDeref = false;
	bool derefDone = false;
	int relIdx = 0;
	int relStartIdx = 0;
	int idx = 0;
	int initDerefIdx = 0;

	length = strlen(pattern);
	patternBytes = new short[length];

	while(*++p && p - pattern <= (long)length) {

		while (*p == ' ') p++;

		if (*p == '?') {
			if (*(p+1) == '?')
				p++;
			patternBytes[idx++] = -1;
		} else if (*p == '[') {
			assert(!inRelDeref && !derefDone);
			inRelDeref = true;
			relStartIdx = idx;
			if (idx) {
				relIdx++;
				operations.emplace_back(pOperation(1, idx));
			}
			operations.emplace_back(pOperation());
		} else if (*p == ']') {
			assert(inRelDeref);
			inRelDeref = false;
			derefDone = true;

			pOperation& op = operations.at(relIdx);

			op.op = 2;
			op.offset = initDerefIdx - relStartIdx;
			op.v1 = idx - relStartIdx;

		} else if (*p == '*') {
			assert(!derefDone);
			derefDone = true;

			initDerefIdx = idx;

			if (!inRelDeref)
				operations.emplace_back(pOperation(0, idx));
			p++;

			while (*p == '+' || *p == '-' || *p == '*') {
				if (*p == '*')
					operations.emplace_back(pOperation(*p++ == '*' ? 0 : 1));
			    else {
					pOperation op = pOperation();
					if (*p == '+' || *p == '-')
						op.offset = strtol(p, &p, 10);
					op.op = (*p == '*') ? 0 : 1;
					if (*p == '*')
						p++;
					operations.emplace_back(op);
				}
			}

		} else
			patternBytes[idx++] = strtoul(p, &p, 16);
	}

	length = idx;
}

uintptr_t PatternScan::FindPattern(const char* pattern, uintptr_t start, uintptr_t end)
{
	short* patternBytes = nullptr;
	size_t length = 0;
	std::vector<pOperation> operations;

	uintptr_t addr = 0;

	ParsePattern(pattern, patternBytes, length, operations);

	for (uintptr_t i = start; i < end - length; i++) {
		bool found = true;
		for (uintptr_t o = 0; o < length && found; o++)
			if (patternBytes[o] >= 0 && *(char*)(i + o) != patternBytes[o])
				found = false;

		if (found) {
			addr = i;
			break;
		}
	}

	if (addr)
		for (auto& i : operations)
			i.RunOp(addr);

	delete[] patternBytes;
	return addr;
}

uintptr_t PatternScan::FindPattern(const char* pattern, const char* module)
{
	ModuleInfo info = Handles::GetModuleInfo(module);
	return FindPattern(pattern, info.address, info.address + info.size);
}
