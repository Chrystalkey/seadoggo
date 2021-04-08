//
// Created by Chrystalkey on 04.02.2021.
//

#ifndef SEADOGGO_ENUMERATIONS_H
#define SEADOGGO_ENUMERATIONS_H

namespace SeaDoggo{
    enum LoginType{
        Samba,
        SFTP,
        FSys,
        LoginError,
        LoginUndefined
    };

    enum LogLevel{
        Debug,
        Info,
        Warning,
        Error,
        NotSet
    };
}

#endif //SEADOGGO_ENUMERATIONS_H
