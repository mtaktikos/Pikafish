/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2022 The Stockfish developers (see AUTHORS file)
  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>

#include "bitboard.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "uci.h"
#include "nnue/evaluate_nnue.h"

using namespace Stockfish;

int biases[16 * 8] {
    1198, -745, 487, 691, -1030, -888, -1511, -1277, -382, 396, 330, 1032, 74, -1829, 545, 249,
    953, 874, -693, -163, -969, 1558, -1510, -460, 1485, 1912, -292, 1288, 1841, -137, 1684, -63,
    688, 546, -1707, -282, -380, -3067, 2001, -1894, -1424, -1832, -1093, -2470, -2319, -2610, 29, -547,
    -662, 1631, -1821, 1460, 268, -2210, -1500, -1168, 768, -824, -1573, 0, -2198, -4133, 818, -274,
    1668, 145, 194, -3389, -1870, -1608, 227, -2692, 768, -124, -209, 2911, -1766, -2619, -4111, -325,
    2136, -1536, -696, 1616, -1332, 1049, 242, -620, -1029, -1157, 446, 3234, 160, -2867, -3937, -87,
    3569, 1933, -1407, 2607, -1197, 535, -3726, 434, 127, -2606, 4647, 705, -76, -2362, 1242, -79,
    3685, -815, 1481, 2602, -3120, -782, -952, -551, -2747, -3695, -751, 879, 1633, -1508, -661, -44,
};

auto myfunc127 = [](int m){return m == 0   ? std::pair<int, int>(-1, 1)
                                    : m >  0 ? std::pair<int, int>(m / 2,3 * m / 2)
                                            : std::pair<int, int>(3 * m / 2,m / 2);};

void postUpdate() {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 16; ++j) {
            if (Eval::NNUE::network[i]) {
                Eval::NNUE::network[i]->fc_0.biases[j] = biases[i * 16 + j];
            }
        }
    }
}

TUNE(SetRange(myfunc127), biases, postUpdate);

int main(int argc, char* argv[]) {

  std::cout << engine_info() << std::endl;

  CommandLine::init(argc, argv);
  UCI::init(Options);
  Tune::init();
  Bitboards::init();
  Position::init();
  Threads.set(size_t(Options["Threads"]));
  Search::clear(); // After threads are up
  Eval::NNUE::init();

//  for (int i = 0; i < 8; ++i) {
//      for (int j = 0; j < 16; ++j) {
//          if (Eval::NNUE::network[i]) {
//              std::cout << Eval::NNUE::network[i]->fc_0.biases[j] << ", ";
//          }
//      }
//      std::cout << std::endl;
//  }

  UCI::loop(argc, argv);

  Threads.set(0);
  return 0;
}
