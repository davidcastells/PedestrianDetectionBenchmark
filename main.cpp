/*
 * Copyright (C) 2018 Universitat Autonoma de Barcelona 
 * David Castells-Rufas <david.castells@uab.cat>
 * 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   main.c
 * Author: dcr
 *
 * Created on February 1, 2018, 10:59 AM
 */

#include <stdio.h>
#include <stdlib.h>

#include "PedestrianDetectionBenchmark.h"

/*
 * 
 */
int main(int argc, char** argv)
{
    PedestrianDetectionBenchmark benchmark;
    
    benchmark.parseOptions(argc, argv);
    benchmark.run();
    
    return (EXIT_SUCCESS);
}

