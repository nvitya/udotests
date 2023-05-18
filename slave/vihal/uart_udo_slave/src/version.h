/*
 *  file:     version.h
 *  brief:    Version tracking and version log for the sample application
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#ifndef SRC_VERSION_H_
#define SRC_VERSION_H_

#define VERSION_MAJOR       1
#define VERSION_MINOR       0
#define VERSION_INCREMENT   0

#define VERSION_INTEGER  (VERSION_MAJOR * 10000 + VERSION_MINOR * 1000 + VERSION_INCREMENT)
#define FIRMWARE_VERSION_INTEGER  VERSION_INTEGER

/*
1.0.0
  - original version
*/

#endif /* SRC_VERSION_H_ */
