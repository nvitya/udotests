/*
 * nvstorage.h
 *
 */

#ifndef SRC_NVSTORAGE_H_
#define SRC_NVSTORAGE_H_

class TNvStorage
{
public:
  bool initialized = false;

  bool Init();
  void Erase(unsigned addr, unsigned len);
  void Write(unsigned addr, void * src, unsigned len);
  void CopyTo(unsigned addr, void * src, unsigned len);  // checks the previous contents first
  void Read(unsigned addr, void * dst, unsigned len);
};

extern TNvStorage g_nvstorage;

#endif /* SRC_NVSTORAGE_H_ */
