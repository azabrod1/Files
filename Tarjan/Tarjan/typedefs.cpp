//
//  typedefs.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/30/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include <stdio.h>
#include "typedefs.h"

/*
CellStatus CellStatus::complete(nullptr);
CellStatus CellStatus::reserved(nullptr);
CellStatus* const CellStatus::COMPLETE_CELL = &complete;
CellStatus* const CellStatus::NEW_CELL      = nullptr;
*/
 

const char CellStatus::COMPLETE  = 2;

const char CellStatus::CONQUERED = 1;

const char CellStatus::OCCUPIED  = 0;

long CellStatus::complete[100];

Search* const CellStatus::NEW_CELL      = nullptr;
Search* const CellStatus::COMPLETE_CELL((Search *) &complete);

int qCount = 0; int qCount2(0);

std::atomic<int> s(0), s2(0);

SimpleClock profiler;
