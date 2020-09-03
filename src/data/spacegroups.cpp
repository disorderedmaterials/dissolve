/*
    *** Crystallographic Spacegroups
    *** src/data/spacegroups.cpp
    Copyright T. Youngs 2019-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "data/spacegroups.h"
#include "base/messenger.h"

/*
 * Spacegroup Name Data
 */

SpacegroupNameData::SpacegroupNameData(std::string_view name, std::string_view formattedName)
    : name_(name), formattedName_(formattedName)
{
}

/*
 * Spacegroups
 */

// Return SpacegroupNameData with index specified
static const SpacegroupNameData &spacegroupNameData(int id)
{
    static SpacegroupNameData spacegroupNameData[] = {
        {"None", "None"},                      //   0
        {"P1", "<i>P</i>1"},                   //   1
        {"P-1", "<i>P</i>-1"},                 //   2
        {"P2", "<i>P</i>2"},                   //   3
        {"P21", "<i>P</i>2<sub>1</sub>"},      //   4
        {"C2", "<i>C</i>2"},                   //   5
        {"Pm", "<i>Pm</i>"},                   //   6
        {"Pc", "<i>Pc</i>"},                   //   7
        {"Cm", "<i>Cm</i>"},                   //   8
        {"Cc", "<i>Cc</i>"},                   //   9
        {"P2/m", "<i>P</i>2<i>/m</i>"},        //  10
        {"P21/m", "<i>P</i>2<sub>1</sub>"},    //  11
        {"C2/m", "<i>C</i>2<i>/m</i>"},        //  12
        {"P2/c", "<i>P</i>2<i>/c</i>"},        //  13
        {"P21/c", "<i>P</i>2<sub>1</sub>"},    //  14
        {"C2/c", "<i>C</i>2<i>/c</i>"},        //  15
        {"P222", "<i>P</i>222"},               //  16
        {"P2221", "<i>P</i>222<sub>1</sub>"},  //  17
        {"P21212", "<i>P</i>2<sub>1</sub>"},   //  18
        {"P212121", "<i>P</i>2<sub>1</sub>"},  //  19
        {"C2221", "<i>C</i>222<sub>1</sub>"},  //  20
        {"C222", "<i>C</i>222"},               //  21
        {"F222", "<i>F</i>222"},               //  22
        {"I222", "<i>I</i>222"},               //  23
        {"I212121", "<i>I</i>2<sub>1</sub>"},  //  24
        {"Pmm2", "<i>Pmm</i>2"},               //  25
        {"Pmc21", "<i>Pmc</i>2<sub>1</sub>"},  //  26
        {"Pcc2", "<i>Pcc</i>2"},               //  27
        {"Pma2", "<i>Pma</i>2"},               //  28
        {"Pca21", "<i>Pca</i>2<sub>1</sub>"},  //  29
        {"Pnc2", "<i>Pnc</i>2"},               //  30
        {"Pmn21", "<i>Pmn</i>2<sub>1</sub>"},  //  31
        {"Pba2", "<i>Pba</i>2"},               //  32
        {"Pna21", "<i>Pna</i>2<sub>1</sub>"},  //  33
        {"Pnn2", "<i>Pnn</i>2"},               //  34
        {"Cmm2", "<i>Cmm</i>2"},               //  35
        {"Cmc21", "<i>Cmc</i>2<sub>1</sub>"},  //  36
        {"Ccc2", "<i>Ccc</i>2"},               //  37
        {"Amm2", "<i>Amm</i>2"},               //  38
        {"Aem2", "<i>Aem</i>2"},               //  39
        {"Ama2", "<i>Ama</i>2"},               //  40
        {"Aea2", "<i>Aea</i>2"},               //  41
        {"Fmm2", "<i>Fmm</i>2"},               //  42
        {"Fdd2", "<i>Fdd</i>2"},               //  43
        {"Imm2", "<i>Imm</i>2"},               //  44
        {"Iba2", "<i>Iba</i>2"},               //  45
        {"Ima2", "<i>Ima</i>2"},               //  46
        {"Pmmm", "<i>Pmmm</i>"},               //  47
        {"Pnnn", "<i>Pnnn</i>"},               //  48
        {"Pccm", "<i>Pccm</i>"},               //  49
        {"Pban", "<i>Pban</i>"},               //  50
        {"Pmma", "<i>Pmma</i>"},               //  51
        {"Pnna", "<i>Pnna</i>"},               //  52
        {"Pmna", "<i>Pmna</i>"},               //  53
        {"Pcca", "<i>Pcca</i>"},               //  54
        {"Pbam", "<i>Pbam</i>"},               //  55
        {"Pccn", "<i>Pccn</i>"},               //  56
        {"Pbcm", "<i>Pbcm</i>"},               //  57
        {"Pnnm", "<i>Pnnm</i>"},               //  58
        {"Pmmn", "<i>Pmmn</i>"},               //  59
        {"Pbcn", "<i>Pbcn</i>"},               //  60
        {"Pbca", "<i>Pbca</i>"},               //  61
        {"Pnma", "<i>Pnma</i>"},               //  62
        {"Cmcm", "<i>Cmcm</i>"},               //  63
        {"Cmce", "<i>Cmce</i>"},               //  64
        {"Cmmm", "<i>Cmmm</i>"},               //  65
        {"Cccm", "<i>Cccm</i>"},               //  66
        {"Cmme", "<i>Cmme</i>"},               //  67
        {"Ccce", "<i>Ccce</i>"},               //  68
        {"Fmmm", "<i>Fmmm</i>"},               //  69
        {"Fddd", "<i>Fddd</i>"},               //  70
        {"Immm", "<i>Immm</i>"},               //  71
        {"Ibam", "<i>Ibam</i>"},               //  72
        {"Ibca", "<i>Ibca</i>"},               //  73
        {"Imma", "<i>Imma</i>"},               //  74
        {"P4", "<i>P</i>4"},                   //  75
        {"P41", "<i>P</i>4<sub>1</sub>"},      //  76
        {"P42", "<i>P</i>4<sub>2</sub>"},      //  77
        {"P43", "<i>P</i>4<sub>3</sub>"},      //  78
        {"I4", "<i>I</i>4"},                   //  79
        {"I41", "<i>I</i>4<sub>1</sub>"},      //  80
        {"P-4", "<i>P</i>-4"},                 //  81
        {"I-4", "<i>I</i>-4"},                 //  82
        {"P4/m", "<i>P</i>4<i>/m</i>"},        //  83
        {"P42/m", "<i>P</i>4<sub>2</sub>"},    //  84
        {"P4/n", "<i>P</i>4<i>/n</i>"},        //  85
        {"P42/n", "<i>P</i>4<sub>2</sub>"},    //  86
        {"I4/m", "<i>I</i>4<i>/m</i>"},        //  87
        {"I41/a", "<i>I</i>4<sub>1</sub>"},    //  88
        {"P422", "<i>P</i>422"},               //  89
        {"P4212", "<i>P</i>42<sub>1</su"},     //  90
        {"P4122", "<i>P</i>4<sub>1</sub>"},    //  91
        {"P41212", "<i>P</i>4<sub>1</sub>"},   //  92
        {"P4222", "<i>P</i>4<sub>2</sub>"},    //  93
        {"P42212", "<i>P</i>4<sub>2</sub>"},   //  94
        {"P4322", "<i>P</i>4<sub>3</sub>"},    //  95
        {"P43212", "<i>P</i>4<sub>3</sub>"},   //  96
        {"I422", "<i>I</i>422"},               //  97
        {"I4122", "<i>I</i>4<sub>1</sub>"},    //  98
        {"P4mm", "<i>P</i>4<i>mm</i>"},        //  99
        {"P4bm", "<i>P</i>4<i>bm</i>"},        // 100
        {"P42cm", "<i>P</i>4<sub>2</sub>"},    // 101
        {"P42nm", "<i>P</i>4<sub>2</sub>"},    // 102
        {"P4cc", "<i>P</i>4<i>cc</i>"},        // 103
        {"P4nc", "<i>P</i>4<i>nc</i>"},        // 104
        {"P42mc", "<i>P</i>4<sub>2</sub>"},    // 105
        {"P42bc", "<i>P</i>4<sub>2</sub>"},    // 106
        {"I4mm", "<i>I</i>4<i>mm</i>"},        // 107
        {"I4cm", "<i>I</i>4<i>cm</i>"},        // 108
        {"I41md", "<i>I</i>4<sub>1</sub>"},    // 109
        {"I41cd", "<i>I</i>4<sub>1</sub>"},    // 110
        {"P-42m", "<i>P</i>-42<i>m</i>"},      // 111
        {"P-42c", "<i>P</i>-42<i>c</i>"},      // 112
        {"P-421m", "<i>P</i>-42<sub>1</sub>"}, // 113
        {"P-421c", "<i>P</i>-42<sub>1</sub>"}, // 114
        {"P-4m2", "<i>P</i>-4<i>m</i>2"},      // 115
        {"P-4c2", "<i>P</i>-4<i>c</i>2"},      // 116
        {"P-4b2", "<i>P</i>-4<i>b</i>2"},      // 117
        {"P-4n2", "<i>P</i>-4<i>n</i>2"},      // 118
        {"I-4m2", "<i>I</i>-4<i>m</i>2"},      // 119
        {"I-4c2", "<i>I</i>-4<i>c</i>2"},      // 120
        {"I-42m", "<i>I</i>-42<i>m</i>"},      // 121
        {"I-42d", "<i>I</i>-42<i>d</i>"},      // 122
        {"P4/mmm", "<i>P</i>4<i>/mmm</i>"},    // 123
        {"P4/mcc", "<i>P</i>4<i>/mcc</i>"},    // 124
        {"P4/nbm", "<i>P</i>4<i>/nbm</i>"},    // 125
        {"P4/nnc", "<i>P</i>4<i>/nnc</i>"},    // 126
        {"P4/mbm", "<i>P</i>4<i>/mbm</i>"},    // 127
        {"P4/mnc", "<i>P</i>4<i>/mnc</i>"},    // 128
        {"P4/nmm", "<i>P</i>4<i>/nmm</i>"},    // 129
        {"P4/ncc", "<i>P</i>4<i>/ncc</i>"},    // 130
        {"P42/mmc", "<i>P</i>4<sub>2</sub>"},  // 131
        {"P42/mcm", "<i>P</i>4<sub>2</sub>"},  // 132
        {"P42/nbc", "<i>P</i>4<sub>2</sub>"},  // 133
        {"P42/nnm", "<i>P</i>4<sub>2</sub>"},  // 134
        {"P42/mbc", "<i>P</i>4<sub>2</sub>"},  // 135
        {"P42/mnm", "<i>P</i>4<sub>2</sub>"},  // 136
        {"P42/nmc", "<i>P</i>4<sub>2</sub>"},  // 137
        {"P42/ncm", "<i>P</i>4<sub>2</sub>"},  // 138
        {"I4/mmm", "<i>I</i>4<i>/mmm</i>"},    // 139
        {"I4/mcm", "<i>I</i>4<i>/mcm</i>"},    // 140
        {"I41/amd", "<i>I</i>4<sub>1</sub>"},  // 141
        {"I41/acd", "<i>I</i>4<sub>1</sub>"},  // 142
        {"P3", "<i>P</i>3"},                   // 143
        {"P31", "<i>P</i>3<sub>1</sub>"},      // 144
        {"P32", "<i>P</i>3<sub>2</sub>"},      // 145
        {"R3", "<i>R</i>3"},                   // 146
        {"P-3", "<i>P</i>-3"},                 // 147
        {"R-3", "<i>R</i>-3"},                 // 148
        {"P312", "<i>P</i>312"},               // 149
        {"P321", "<i>P</i>321"},               // 150
        {"P3112", "<i>P</i>3<sub>1</sub>"},    // 151
        {"P3121", "<i>P</i>3<sub>1</sub>"},    // 152
        {"P3212", "<i>P</i>3<sub>2</sub>"},    // 153
        {"P3221", "<i>P</i>3<sub>2</sub>"},    // 154
        {"R32", "<i>R</i>32"},                 // 155
        {"P3m1", "<i>P</i>3<i>m</i>1"},        // 156
        {"P31m", "<i>P</i>31<i>m</i>"},        // 157
        {"P3c1", "<i>P</i>3<i>c</i>1"},        // 158
        {"P31c", "<i>P</i>31<i>c</i>"},        // 159
        {"R3m", "<i>R</i>3<i>m</i>"},          // 160
        {"R3c", "<i>R</i>3<i>c</i>"},          // 161
        {"P-31m", "<i>P</i>-31<i>m</i>"},      // 162
        {"P-31c", "<i>P</i>-31<i>c</i>"},      // 163
        {"P-3m1", "<i>P</i>-3<i>m</i>1"},      // 164
        {"P-3c1", "<i>P</i>-3<i>c</i>1"},      // 165
        {"R-3m", "<i>R</i>-3<i>m</i>"},        // 166
        {"R-3c", "<i>R</i>-3<i>c</i>"},        // 167
        {"P6", "<i>P</i>6"},                   // 168
        {"P61", "<i>P</i>6<sub>1</sub>"},      // 169
        {"P65", "<i>P</i>6<sub>5</sub>"},      // 170
        {"P62", "<i>P</i>6<sub>2</sub>"},      // 171
        {"P64", "<i>P</i>6<sub>4</sub>"},      // 172
        {"P63", "<i>P</i>6<sub>3</sub>"},      // 173
        {"P-6", "<i>P</i>-6"},                 // 174
        {"P6/m", "<i>P</i>6<i>/m</i>"},        // 175
        {"P63/m", "<i>P</i>6<sub>3</sub>"},    // 176
        {"P622", "<i>P</i>622"},               // 177
        {"P6122", "<i>P</i>6<sub>1</sub>"},    // 178
        {"P6522", "<i>P</i>6<sub>5</sub>"},    // 179
        {"P6222", "<i>P</i>6<sub>2</sub>"},    // 180
        {"P6422", "<i>P</i>6<sub>4</sub>"},    // 181
        {"P6322", "<i>P</i>6<sub>3</sub>"},    // 182
        {"P6mm", "<i>P</i>6<i>mm</i>"},        // 183
        {"P6cc", "<i>P</i>6<i>cc</i>"},        // 184
        {"P63cm", "<i>P</i>6<sub>3</sub>"},    // 185
        {"P63mc", "<i>P</i>6<sub>3</sub>"},    // 186
        {"P-6m2", "<i>P</i>-6<i>m</i>2"},      // 187
        {"P-6c2", "<i>P</i>-6<i>c</i>2"},      // 188
        {"P-62m", "<i>P</i>-62<i>m</i>"},      // 189
        {"P-62c", "<i>P</i>-62<i>c</i>"},      // 190
        {"P6/mmm", "<i>P</i>6<i>/mmm</i>"},    // 191
        {"P6/mcc", "<i>P</i>6<i>/mcc</i>"},    // 192
        {"P63/mcm", "<i>P</i>6<sub>3</sub>"},  // 193
        {"P63/mmc", "<i>P</i>6<sub>3</sub>"},  // 194
        {"P23", "<i>P</i>23"},                 // 195
        {"F23", "<i>F</i>23"},                 // 196
        {"I23", "<i>I</i>23"},                 // 197
        {"P213", "<i>P</i>2<sub>1</sub>"},     // 198
        {"I213", "<i>I</i>2<sub>1</sub>"},     // 199
        {"Pm-3", "<i>Pm</i>-3"},               // 200
        {"Pn-3", "<i>Pn</i>-3"},               // 201
        {"Fm-3", "<i>Fm</i>-3"},               // 202
        {"Fd-3", "<i>Fd</i>-3"},               // 203
        {"Im-3", "<i>Im</i>-3"},               // 204
        {"Pa-3", "<i>Pa</i>-3"},               // 205
        {"Ia-3", "<i>Ia</i>-3"},               // 206
        {"P432", "<i>P</i>432"},               // 207
        {"P4232", "<i>P</i>4<sub>2</sub>"},    // 208
        {"F432", "<i>F</i>432"},               // 209
        {"F4132", "<i>F</i>4<sub>1</sub>"},    // 210
        {"I432", "<i>I</i>432"},               // 211
        {"P4332", "<i>P</i>4<sub>3</sub>"},    // 212
        {"P4132", "<i>P</i>4<sub>1</sub>"},    // 213
        {"I4132", "<i>I</i>4<sub>1</sub>"},    // 214
        {"P-43m", "<i>P</i>-43<i>m</i>"},      // 215
        {"F-43m", "<i>F</i>-43<i>m</i>"},      // 216
        {"I-43m", "<i>I</i>-43<i>m</i>"},      // 217
        {"P-43n", "<i>P</i>-43<i>n</i>"},      // 218
        {"F-43c", "<i>F</i>-43<i>c</i>"},      // 219
        {"I-43d", "<i>I</i>-43<i>d</i>"},      // 220
        {"Pm-3m", "<i>Pm</i>-3<i>m</i>"},      // 221
        {"Pn-3n", "<i>Pn</i>-3<i>n</i>"},      // 222
        {"Pm-3n", "<i>Pm</i>-3<i>n</i>"},      // 223
        {"Pn-3m", "<i>Pn</i>-3<i>m</i>"},      // 224
        {"Fm-3m", "<i>Fm</i>-3<i>m</i>"},      // 225
        {"Fm-3c", "<i>Fm</i>-3<i>c</i>"},      // 226
        {"Fd-3m", "<i>Fd</i>-3<i>m</i>"},      // 227
        {"Fd-3c", "<i>Fd</i>-3<i>c</i>"},      // 228
        {"Im-3m", "<i>Im</i>-3<i>m</i>"},      // 229
        {"Ia-3d", "<i>Ia</i>-3<i>d</i>"}       // 230
    };

    if ((id < 0) || (id > 230))
    {
        Messenger::error("Spacegroups::spacegroupNameData() - Spacegroup with id={} is out of range!\n", id);
        return spacegroupNameData[0];
    }

    return spacegroupNameData[id];
};
