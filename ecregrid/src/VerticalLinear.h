/***************************** LICENSE START ***********************************

 Copyright 2012 ECMWF. This software is distributed under the terms
 of the Apache License version 2.0. In applying this license, ECMWF does not
 waive the privileges and immunities granted to it by virtue of its status as
 an Intergovernmental Organization or submit itself to any jurisdiction.

 ***************************** LICENSE END *************************************/

#ifndef VerticalLinear_H
#define VerticalLinear_H

#ifndef   VerticalInterpolator_H
#include "VerticalInterpolator.h"
#endif

// Forward declarations


class VerticalLinear : public VerticalInterpolator {
  public:

// -- Contructors
    VerticalLinear();

// -- Destructor
    virtual ~VerticalLinear();

// -- Methods

    void interpolate(const GridField& inputUp, const GridField& inputDown, double* values, unsigned long valuesSize) const;


  protected:
// -- Methods
    virtual void print(ostream&) const;

// Members

  private:

// -- Friends
    friend ostream& operator<<(ostream& s,const VerticalLinear& p) {
        p.print(s);
        return s;
    }
};

#endif
