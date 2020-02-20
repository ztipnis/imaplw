# imaplw - WIP [![Build Status](https://travis-ci.com/ztipnis/imaplw.svg?branch=master)](https://travis-ci.com/ztipnis/imaplw) ![GitHub top language](https://img.shields.io/github/languages/top/ztipnis/imaplw) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/1b53779bb2554982836ead4d37256113)](https://app.codacy.com/manual/ztipnis/imaplw?utm_source=github.com&utm_medium=referral&utm_content=ztipnis/imaplw&utm_campaign=Badge_Grade_Dashboard)

## Description
A Lightweight IMAP server build in modern C++

## Goals
The goal of this project is to balance extensibility with vision and footprint: this means that the application should be easily extended (i.e. to add additional IMAP commands), but also to not do anything beyond what is outlined by the IETF, and to utilize the minimum possible resources.
This is achieved mainly through:
*   MVC-esq architecture: The program is broken up into a command parser/router (IMAPProvider), an Authentication Manager, and a Data Manager. The Authentiction and Data managers are abstract classes which are subclassed to implement the actual functionality.
*   Modern C++ (17) with a focus on code readability over all else.
*   Event-based architecture achieved through EPoll and KQueue

## A Note on Licensing
This project relies on LibreSSL and Boost for some minor parts of its functionality. LibreSSL and Boost are each licensed under permissive licenses, and the remainder of the code is licensed under the MIT License. For license inquiries, please open an issue. For more info on the specific licenses, see LibreSSL.License, Boost.License, and LICENSE
