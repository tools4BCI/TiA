/*
    This file is part of the TOBI Interface A (TiA) library.

    Commercial Usage
    Licensees holding valid Graz University of Technology Commercial
    licenses may use this file in accordance with the Graz University
    of Technology Commercial License Agreement provided with the
    Software or, alternatively, in accordance with the terms contained in
    a written agreement between you and Graz University of Technology.

    --------------------------------------------------

    GNU Lesser General Public License Usage
    Alternatively, this file may be used under the terms of the GNU Lesser
    General Public License version 3.0 as published by the Free Software
    Foundation and appearing in the file lgpl.txt included in the
    packaging of this file.  Please review the following information to
    ensure the GNU General Public License version 3.0 requirements will be
    met: http://www.gnu.org/copyleft/lgpl.html.

    In case of GNU Lesser General Public License Usage ,the TiA library
    is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the TiA library. If not, see <http://www.gnu.org/licenses/>.

    Copyright 2010 Graz University of Technology
    Contact: TiA@tobi-project.org
*/

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include "tia_exceptions.h"

#include <string>
#include <sstream>

namespace tia
{

//-----------------------------------------------------------------------------
/// removes whitespaces at the beginning and the end of the given string
inline std::string trim (std::string str)
{
    size_t start_index = 0;
    size_t end_index = str.size () - 1;

    while ((str[start_index] == ' ') && (start_index <= end_index))
        start_index++;

    while ((str[end_index] == ' ') && (end_index >= start_index))
        end_index--;

    return str.substr (start_index, end_index - start_index + 1);
}

//-----------------------------------------------------------------------------
inline unsigned toUnsigned (std::string const& str)
{
    std::istringstream iss (str);
    unsigned value;
    iss >> value;
    if (iss.fail())
        throw TiAException (std::string ("Failed to convert \"") + str + "\" into an unsigned int.");
    return value;
}

//-----------------------------------------------------------------------------
inline std::string toString (unsigned number)
{
    std::ostringstream oss;
    oss << std::dec << number;
    if (oss.fail())
        throw TiAException ("Failed to a number into a string.");
    return oss.str ();
}


}

#endif // STRING_UTILS_H
