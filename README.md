# Game updater Project

This project can help player update their minecraft file.

## How to use

* Download `game-update.exe` obtained from server maintainers.
* Move execute file to your minecraft folder.
* Run `game-update.exe`.

## Server-side requirements

filename | type
----|-----
`unzip.exe` | standalone
`sha256sum.zip` | package, included runtime dymanic-link library

All requirements can be download in [releases](https://github.com/rubbish-mc/game-updater/releases/tag/1.0).

Notice: Server-side json must using the latest `gen.py` generate.

## Compile

Using visual studio to compile, with `boost::filesystem` support, no other requirements. You need change `base_website` variable to you own website (copy `dlwebsite.h.default` to `dlwebsite.h`, then edit it).

Compile passed in Visual Studio 15.7.1 with VC++ 2017.

## License

[![](https://www.gnu.org/graphics/agplv3-155x51.png)](https://www.gnu.org/licenses/agpl-3.0.txt)

Copyright (C) 2018 Too-Naive

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.