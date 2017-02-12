//
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2013 Project Chrono
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file at the top level of the distribution
// and at http://projectchrono.org/license-chrono.txt.
//

#ifndef CHARCHIVEBINARY_H
#define CHARCHIVEBINARY_H

#include "chrono/serialization/ChArchive.h"
#include "chrono/core/ChLog.h"

namespace chrono {

///
/// This is a class for serializing to binary archives
///

class  ChArchiveOutBinary : public ChArchiveOut {
  public:

      ChArchiveOutBinary( ChStreamOutBinary& mostream) {
          ostream = &mostream;
      };

      virtual ~ChArchiveOutBinary() {};

      virtual void out     (ChNameValue<bool> bVal) {
            (*ostream) << bVal.value();
      }
      virtual void out     (ChNameValue<int> bVal) {
            (*ostream) << bVal.value();
      }
      virtual void out     (ChNameValue<double> bVal) {
            (*ostream) << bVal.value();
      }
      virtual void out     (ChNameValue<float> bVal){
            (*ostream) << bVal.value();
      }
      virtual void out     (ChNameValue<char> bVal){
            (*ostream) << bVal.value();
      }
      virtual void out     (ChNameValue<unsigned int> bVal){
            (*ostream) << bVal.value();
      }
      virtual void out     (ChNameValue<const char*> bVal){
            (*ostream) << bVal.value();
      }
      virtual void out     (ChNameValue<std::string> bVal){
            (*ostream) << bVal.value();
      }
      virtual void out     (ChNameValue<unsigned long> bVal){
            (*ostream) << bVal.value();
      }
      virtual void out     (ChNameValue<unsigned long long> bVal){
            (*ostream) << bVal.value();
      }
      virtual void out     (ChNameValue<ChEnumMapperBase> bVal) {
            (*ostream) << bVal.value().GetValueAsInt();
      }

      virtual void out_array_pre (const char* name, size_t msize, const char* classname) {
            (*ostream) << msize;
      }
      virtual void out_array_between (size_t msize, const char* classname) {}
      virtual void out_array_end (size_t msize,const char* classname) {}


        // for custom c++ objects:
      virtual void out     (ChNameValue<ChFunctorArchiveOut> bVal, const char* classname, bool tracked, size_t obj_ID) {
          bVal.value().CallArchiveOut(*this);
      }

        // for pointed objects (if pointer hasn't been already serialized, otherwise save offset)
      virtual void out_ref_polimorphic (ChNameValue<ChFunctorArchiveOut> bVal, bool already_inserted, size_t obj_ID, const char* classname) 
      {
          if (!already_inserted) {
            // New Object, we have to full serialize it
            std::string str(classname);
            (*ostream) << str;    // serialize class type as string (platform/compiler independent), for class factory later
            bVal.value().CallArchiveOutConstructor(*this);
            bVal.value().CallArchiveOut(*this);
          } else {
            // Object already in list. Only store obj_ID as ID
            std::string str("oID");
            (*ostream) << str;       // serialize 'this was already saved' info as "oID" string
            (*ostream) << obj_ID;    // serialize obj_ID in pointers vector as ID
          }
      }

      virtual void out_ref          (ChNameValue<ChFunctorArchiveOut> bVal, bool already_inserted, size_t obj_ID,  const char* classname) 
      {
          if (!already_inserted) {
            // New Object, we have to full serialize it
            std::string str(""); 
            (*ostream) << str;    // not usable for class factory: serialize as "" class type
            bVal.value().CallArchiveOutConstructor(*this);
            bVal.value().CallArchiveOut(*this);
          } else {
            // Object already in list. Only store obj_ID
            std::string str("oID");
            (*ostream) << str;       // serialize 'this was already saved' info
            (*ostream) << obj_ID;    // serialize obj_ID in pointers vector
          }
      }

  protected:
      ChStreamOutBinary* ostream;
};





///
/// This is a class for for serializing from binary archives
///

class  ChArchiveInBinary : public ChArchiveIn {
  public:

      ChArchiveInBinary( ChStreamInBinary& mistream) {
          istream = &mistream;
      };

      virtual ~ChArchiveInBinary() {};

      virtual void in     (ChNameValue<bool> bVal) {
            (*istream) >> bVal.value();
      }
      virtual void in     (ChNameValue<int> bVal) {
            (*istream) >> bVal.value();
      }
      virtual void in     (ChNameValue<double> bVal) {
            (*istream) >> bVal.value();
      }
      virtual void in     (ChNameValue<float> bVal){
            (*istream) >> bVal.value();
      }
      virtual void in     (ChNameValue<char> bVal){
            (*istream) >> bVal.value();
      }
      virtual void in     (ChNameValue<unsigned int> bVal){
            (*istream) >> bVal.value();
      }
      virtual void in     (ChNameValue<std::string> bVal){
            (*istream) >> bVal.value();
      }
      virtual void in     (ChNameValue<unsigned long> bVal){
            (*istream) >> bVal.value();
      }
      virtual void in     (ChNameValue<unsigned long long> bVal){
            (*istream) >> bVal.value();
      }
      virtual void in     (ChNameValue<ChEnumMapperBase> bVal) {
            int foo;
            (*istream) >>  foo;
            bVal.value().SetValueAsInt(foo);
      }
         // for wrapping arrays and lists
      virtual void in_array_pre (const char* name, size_t& msize) {
            (*istream) >> msize;
      }
      virtual void in_array_between (const char* name) {}
      virtual void in_array_end (const char* name) {}

        //  for custom c++ objects:
      virtual void in     (ChNameValue<ChFunctorArchiveIn> bVal) {
          if (bVal.flags() & NVP_TRACK_OBJECT){
              bool already_stored; size_t obj_ID;
              PutPointer(bVal.value().CallGetRawPtr(*this), already_stored, obj_ID);
          }
          bVal.value().CallArchiveIn(*this);
      }

      // for pointed objects 
      virtual void in_ref_polimorphic (ChNameValue<ChFunctorArchiveIn> bVal) 
      {
          std::string cls_name;
          (*istream) >> cls_name;

          if (!(cls_name == "oID")) {
            // Dynamically create using class factory:
            // call new(), or deserialize constructor params+call new():
            bVal.value().CallArchiveInConstructor(*this, cls_name.c_str()); 

            if (bVal.value().CallGetRawPtr(*this)) {
                bool already_stored; size_t obj_ID;
                PutPointer(bVal.value().CallGetRawPtr(*this), already_stored, obj_ID);
                // 3) Deserialize
                bVal.value().CallArchiveIn(*this);
            } else {
                throw(ChExceptionArchive("Archive cannot create polimorphic object \'" + cls_name + "\' " ));
            }

          } else {
            size_t obj_ID = 0;
            // Was a shared object: just get the pointer to already-retrieved
            (*istream) >> obj_ID;

            bVal.value().CallSetRawPtr(*this, objects_pointers[obj_ID]);
          }
      }

      virtual void in_ref          (ChNameValue<ChFunctorArchiveIn> bVal)
      {
          std::string cls_name;
          (*istream) >> cls_name;

          if (!(cls_name == "oID")) {
            // Dynamically create (no class factory will be invoked for non-polimorphic obj):
            // call new(), or deserialize constructor params+call new():
            bVal.value().CallArchiveInConstructor(*this, cls_name.c_str()); 

            if (bVal.value().CallGetRawPtr(*this)) {
                bool already_stored; size_t obj_ID;
                PutPointer(bVal.value().CallGetRawPtr(*this), already_stored, obj_ID);
                // 3) Deserialize
                bVal.value().CallArchiveIn(*this);
            } else {
                throw(ChExceptionArchive("Archive cannot create object"));
            }

          } else {
            size_t obj_ID = 0;
            //  Was a shared object: just get the pointer to already-retrieved
            (*istream) >> obj_ID;

            bVal.value().CallSetRawPtr(*this, objects_pointers[obj_ID]);
          }
      }

  protected:
      ChStreamInBinary* istream;
};

}  // end namespace chrono

#endif
