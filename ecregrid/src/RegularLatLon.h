/***************************** LICENSE START ***********************************

 Copyright 2012 ECMWF. This software is distributed under the terms
 of the Apache License version 2.0. In applying this license, ECMWF does not
 waive the privileges and immunities granted to it by virtue of its status as
 an Intergovernmental Organization or submit itself to any jurisdiction.

 ***************************** LICENSE END *************************************/

#ifndef RegularLatLon_H
#define RegularLatLon_H


#ifndef LatLon_H
#include "LatLon.h"
#endif

#ifndef Regular_H
#include "Regular.h"
#endif

/*! \class RegularLatLon
    \brief Regular LatLon Grid

*/

class RegularLatLon : public LatLon, public Regular {
  public:

// -- Exceptions
    // None

// -- Contructors

    RegularLatLon(double north, double west, double south, double east, double ns, double we);
    explicit RegularLatLon(double ns, double we);
    explicit RegularLatLon(long ns, long we);
    RegularLatLon(const Area& other, double ns, double we);

    RegularLatLon(double north, double west, double south, double east, double ns, double we, long nsNpts, long weNpts);
    RegularLatLon(const Area& other, double ns, double we, long nsNpts, long weNpts);
    RegularLatLon(double north, double west, double south, double east, long nsNpts, long weNpts, bool globalWE);
    RegularLatLon(const Area& other, long nsNpts, long weNpts, bool globalWE);

// -- Destructor

    ~RegularLatLon(); // Change to virtual if base class

// -- Operators
    // None

// -- Methods
    // None

// -- Overridden methods


  protected:

// -- Members
    // None

// -- Methods
    void print(ostream&) const; // Change to virtual if base class

// -- Overridden methods
    // None

// -- Class members
    // None


  private:

// No copy allowed

    RegularLatLon(const RegularLatLon&);
    RegularLatLon& operator=(const RegularLatLon&);

// -- Members
    // None

// -- Methods
    // None

// -- Overridden methods
    Grid* newGrid(const Grid& other) const ;
    Grid* newGrid(const Area& other) const ;
    Grid* getGlobalGrid()            const ;

    void nearestPoints(GridContext*,const Point& where, vector<FieldPoint>& nearests, const vector<double>& data, int scMode, int npts) const;

    double conserving(GridContext* ctx, const Point& where, const vector<double>& data, int scMode, double missingValue, double incNs, double incWe)  const;

    void cellsAreas(vector<Area>& areas, vector<double>& areasSize)  const;
    double fluxConserving(GridContext* ctx, const Point& where, const vector<Area>& areas, const vector<double>& inputCellSize, const double* data, int scMode, double missingValue, double outNSincrement, double outWEincrement)  const;

    double averageWeighted(GridContext* ctx, const Point& where, const vector<double>& weights, const vector<double>& data, int scMode, double missingValue, const vector<double>& outLats, double incWe)  const;
    double averageWeightedLsm(GridContext* ctx, const Point& where, const vector<double>& weights, const vector<double>& data,const vector<double>& dataLsmIn, const vector<double>& dataLsmOut, int scMode, double missingValue, const vector<double>& outLats, double incWe)  const;

    void nearest4pts(GridContext* ctx,const Point& where, vector<Point>& result) const;
    void nearestsByIndex(const Point& where, vector<FieldPoint>& nearest, const vector<double>& data, int scMode, double missingValue, int howMany) const;

    virtual void generateGrid1D(vector<Point>&)    const ;
    virtual void generateGrid1D(vector<Point>& ll, const Grid& grid)    const ;

    unsigned long calculatedNumberOfPoints()   const ;
    size_t  getGridDefinition(vector<long>& v) const;

    virtual string   composedName()  const {
        return "regular_ll";
    }

    int    getLatitudeOffset(double lat) const;
    long   getLatitudeOffset(double lat, long& current) const;
    void   getOffsets(vector<int>& offsets) const;

    virtual GridContext* getGridContext() const {
        return Regular::getGridContext();
    }
    virtual bool sameAs(const  RegularLatLon& other) const  ;
    virtual bool sameAs(const RegularLatLonCellCentered& other)   const  {
        return LatLon::sameAs(other);
    }
    virtual bool sameAs(const ReducedLatLon& other)   const  {
        return LatLon::sameAs(other);
    }
    virtual bool sameAs(const RotatedRegularLatLon& other)   const  {
        return LatLon::sameAs(other);
    }
    virtual bool sameAs(const ReducedGaussian& other) const  {
        return LatLon::sameAs(other);
    }
    virtual bool sameAs(const RegularGaussian& other) const  {
        return LatLon::sameAs(other);
    }
    virtual bool sameAs(const PseudoRegularGaussian& other) const  {
        return LatLon::sameAs(other);
    }
    virtual bool sameAs(const PolarStereographic& other) const  {
        return LatLon::sameAs(other);
    }
    virtual bool sameAs(const ListOfPoints& other)    const  {
        return LatLon::sameAs(other);
    }
    virtual bool equals(const Grid& other) const {
        return other.sameAs(*this);
    }

    virtual void reOrderNewData(const vector<double>& data_in, vector<double>& data_out, int scMode) const;

    string   predefinedLsmFileName() const;

    void addWeightsAlongLatitude(vector<double>& weights, double weightY, long i) const;
    long getIndex(int i, int j) const;

    void accumulatedDataWE(const double* data, unsigned long dataLength, vector<double>& newData) const;
    void getGridAsLatLonList(double* lats, double* lons, long* length) const;

    void findWestAndEastIndex(GridContext* ctx, long j, double west, double east, int& westLongitudeIndex, int& eastLongitudeIndex ) const;

// -- Friends

    friend ostream& operator<<(ostream& s,const RegularLatLon& p) {
        p.print(s);
        return s;
    }

};

#endif
