/*
 * enums.h
 * This file is part of graphSampler
 *
 * Copyright (C) 2017 - Giacomo Bergami
 * Created by Giacomo Bergami on 16/08/16.
 *
 * graphSampler is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * graphSampler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with graphSampler. If not, see <http://www.gnu.org/licenses/>.
 */



#pragma once

namespace yaucl {
    namespace structures {
        namespace rbtree {

            /**
             * Defines the current status of the iteration
             */
            enum class PositionIteratorEnum : char {
                Left = -1, Center = 0 ,Right = 1, None = -2
            };

            /**
             * Defines the color of the current subtree's root
             */
            enum class Color { RED, BLACK };

        }
    }
}
