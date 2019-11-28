#pragma once
#include <acgi.h>
#include <migrtion.h>
#include <dbapserv.h>
#include <dbmain.h>
#include <id.h>

//AcGiViewportGeometry代理
class IGiViewportGeometry
{
public:
	virtual Adesk::Boolean polylineEye(int eval, const Adesk::UInt32 nbPoints, const AcGePoint3d* pPoints) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polygonEye(int eval, const Adesk::UInt32 nbPoints, const AcGePoint3d* pPoints) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polylineDc(int eval, const Adesk::UInt32 nbPoints, const AcGePoint3d* pPoints) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polygonDc(int eval, const Adesk::UInt32 nbPoints, const AcGePoint3d* pPoints) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean rasterImageDc(int eval, const AcGePoint3d& origin, const AcGeVector3d& u, const AcGeVector3d& v, 
		const AcGeMatrix2d& pixelToDc, AcDbObjectId entityId, AcGiImageOrg imageOrg, Adesk::UInt32 imageWidth, 
		Adesk::UInt32 imageHeight, Adesk::Int16 imageColorDepth, Adesk::Boolean transparency, AcGiViewportGeometry::ImageSource source, 
		const AcGeVector3d& unrotatedU, const AcGiImageOrg origionalImageOrg, const AcGeMatrix2d& unrotatedPixelToDc, 
		const Adesk::UInt32 unrotatedImageWidth, const Adesk::UInt32 unrotatedImageHeight) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean ownerDrawDc(int eval, Adesk::Int32 vpnumber, Adesk::Int32 left, Adesk::Int32 top, Adesk::Int32 right, 
		Adesk::Int32 bottom, const OwnerDraw* pOwnerDraw) /*const*/ 
	{ return Adesk::kTrue; }

	virtual void getModelToWorldTransform(int eval, AcGeMatrix3d& mat) /*const*/ { }

	virtual void getWorldToModelTransform(int eval, AcGeMatrix3d& mat) /*const*/ { }

	virtual Adesk::Boolean pushModelTransform(int eval, const AcGeVector3d & vNormal) 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean pushModelTransform(int eval, const AcGeMatrix3d & xMat) 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean popModelTransform(int eval) 
	{ return Adesk::kTrue; }

	virtual AcGeMatrix3d pushPositionTransform(int eval, AcGiPositionTransformBehavior behavior, const AcGePoint3d& offset) 
	{ return AcGeMatrix3d(); }

	virtual AcGeMatrix3d pushPositionTransform(int eval, AcGiPositionTransformBehavior behavior, const AcGePoint2d& offset) 
	{ return AcGeMatrix3d(); }

	virtual AcGeMatrix3d pushScaleTransform(int eval, AcGiScaleTransformBehavior behavior, const AcGePoint3d& extents) 
	{ return AcGeMatrix3d(); }

	virtual AcGeMatrix3d pushScaleTransform(int eval, AcGiScaleTransformBehavior behavior, const AcGePoint2d& extents) 
	{ return AcGeMatrix3d(); }

	virtual AcGeMatrix3d pushOrientationTransform(int eval, AcGiOrientationTransformBehavior behavior) 
	{ return AcGeMatrix3d(); }

	virtual Adesk::Boolean circle(int eval, const AcGePoint3d& center, const double radius, const AcGeVector3d& normal) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean circle(int eval, const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& pt3) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean circularArc(int eval, const AcGePoint3d& center, const double radius, const AcGeVector3d& normal,
		const AcGeVector3d& startVector, const double sweepAngle, const AcGiArcType arcType = kAcGiArcSimple) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean circularArc(int eval, const AcGePoint3d& start, const AcGePoint3d& point, const AcGePoint3d& end, const AcGiArcType arcType = kAcGiArcSimple) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polyline(int eval, const Adesk::UInt32 nbPoints, const AcGePoint3d* pVertexList, 
		const AcGeVector3d* pNormal = NULL, Adesk::LongPtr lBaseSubEntMarker = -1) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polyline(int eval, const AcGiPolyline& polylineObj) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polyPolyline(int eval, Adesk::UInt32 nbPolylines, const AcGiPolyline* pPolylines) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polygon(int eval, const Adesk::UInt32 nbPoints, const AcGePoint3d* pVertexList) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polyPolygon(int eval, const Adesk::UInt32 numPolygonIndices, const Adesk::UInt32* numPolygonPositions, 
		const AcGePoint3d* polygonPositions, const Adesk::UInt32* numPolygonPoints, const AcGePoint3d* polygonPoints, 
		const AcCmEntityColor* outlineColors = NULL, const AcGiLineType* outlineTypes = NULL, 
		const AcCmEntityColor* fillColors = NULL, const AcCmTransparency* fillOpacities = NULL) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean mesh(int eval, const Adesk::UInt32 rows, const Adesk::UInt32 columns, const AcGePoint3d* pVertexList, 
		const AcGiEdgeData* pEdgeData = NULL, const AcGiFaceData* pFaceData = NULL, 
		const AcGiVertexData* pVertexData = NULL, const bool bAutoGenerateNormals = true) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean shell(int eval, const Adesk::UInt32 nbVertex, const AcGePoint3d* pVertexList, const Adesk::UInt32 faceListSize,
		const Adesk::Int32* pFaceList, const AcGiEdgeData* pEdgeData = NULL, const AcGiFaceData* pFaceData = NULL, 
		const AcGiVertexData* pVertexData = NULL, const struct resbuf* pResBuf = NULL, const bool bAutoGenerateNormals = true) /*const*/ 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean text(int eval, const AcGePoint3d& position, const AcGeVector3d& normal, const AcGeVector3d& direction, 
		const double height, const double width, const double oblique, const ACHAR* pMsg) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean text(int eval, const AcGePoint3d& position, const AcGeVector3d& normal, const AcGeVector3d& direction, 
		const ACHAR* pMsg, const Adesk::Int32 length, const Adesk::Boolean raw, const AcGiTextStyle &pTextStyle) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean xline(int eval, const AcGePoint3d& pt1, const AcGePoint3d& pt2) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean ray(int eval, const AcGePoint3d& pt1, const AcGePoint3d& pt2) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean pline(int eval, const AcDbPolyline& lwBuf, Adesk::UInt32 fromIndex = 0, Adesk::UInt32 numSegs = 0) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean draw(int eval, AcGiDrawable*) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean image(int eval, const AcGiImageBGRA32& imageSource, const AcGePoint3d& position, 
		const AcGeVector3d& u, const AcGeVector3d& v, AcGiGeometry::TransparencyMode transparencyMode = AcGiGeometry::kTransparency8Bit) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean rowOfDots(int eval, int count, const AcGePoint3d& start, const AcGeVector3d& step) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean ellipticalArc(int eval, const AcGePoint3d& center, const AcGeVector3d& normal, double majorAxisLength, double minorAxisLength, 
		double startDegreeInRads, double endDegreeInRads, double tiltDegreeInRads, AcGiArcType arcType = kAcGiArcSimple) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean pushClipBoundary(int eval, AcGiClipBoundary * pBoundary) 
	{ return Adesk::kTrue; }

	virtual void popClipBoundary(int eval) { }

	virtual Adesk::Boolean worldLine(int eval, const AcGePoint3d pnts[2]) 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polypoint(int eval, const Adesk::UInt32 nbPoints, const AcGePoint3d* pVertexList, 
		const AcGeVector3d* pNormal = NULL, const Adesk::LongPtr *pSubEntMarkers = NULL) /*const*/
	{ return Adesk::kTrue; }

#if	(ACADV_RELMAJOR >= 20)
	virtual Adesk::Boolean edge(int eval, const AcArray<AcGeCurve2d *>& edges) /*const*/
	{ return Adesk::kTrue; }
#endif
};

//AcGiWorldGeometry代理
class IGiWorldGeometry
{
public:
	virtual void setExtents(int eval, AcGePoint3d *pNewExtents) /*const*/{ }

	virtual void getModelToWorldTransform(int eval, AcGeMatrix3d&) /*const*/{ }

	virtual void getWorldToModelTransform(int eval, AcGeMatrix3d&) /*const*/{ }

	virtual Adesk::Boolean pushModelTransform(int eval, const AcGeVector3d & vNormal) 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean pushModelTransform(int eval, const AcGeMatrix3d & xMat) 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean popModelTransform(int eval) 
	{ return Adesk::kTrue; }

	virtual AcGeMatrix3d pushPositionTransform(int eval, AcGiPositionTransformBehavior behavior, const AcGePoint3d& offset) 
	{ return AcGeMatrix3d(); }

	virtual AcGeMatrix3d pushPositionTransform(int eval, AcGiPositionTransformBehavior behavior, const AcGePoint2d& offset) 
	{ return AcGeMatrix3d(); }

	virtual AcGeMatrix3d pushScaleTransform(int eval, AcGiScaleTransformBehavior behavior, const AcGePoint3d& extents) 
	{ return AcGeMatrix3d(); }

	virtual AcGeMatrix3d pushScaleTransform(int eval, AcGiScaleTransformBehavior behavior, const AcGePoint2d& extents) 
	{ return AcGeMatrix3d(); }

	virtual AcGeMatrix3d pushOrientationTransform(int eval, AcGiOrientationTransformBehavior behavior) 
	{ return AcGeMatrix3d(); }

	virtual Adesk::Boolean circle(int eval, const AcGePoint3d& center, const double radius, const AcGeVector3d& normal) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean circle(int eval, const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& pt3) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean circularArc(int eval, const AcGePoint3d& center, const double radius, const AcGeVector3d& normal, const AcGeVector3d& startVector, const double sweepAngle, const AcGiArcType arcType = kAcGiArcSimple) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean circularArc(int eval, const AcGePoint3d& start, const AcGePoint3d& point, const AcGePoint3d& end, const AcGiArcType arcType = kAcGiArcSimple) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polyline(int eval, const Adesk::UInt32 nbPoints, const AcGePoint3d* pVertexList, const AcGeVector3d* pNormal = NULL, Adesk::LongPtr lBaseSubEntMarker = -1) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polyline(int eval, const AcGiPolyline& polylineObj) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polyPolyline(int eval, Adesk::UInt32 nbPolylines, const AcGiPolyline* pPolylines) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polygon(int eval, const Adesk::UInt32 nbPoints, const AcGePoint3d* pVertexList) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polyPolygon(int eval, const Adesk::UInt32 numPolygonIndices, const Adesk::UInt32* numPolygonPositions, 
		const AcGePoint3d* polygonPositions, const Adesk::UInt32* numPolygonPoints, const AcGePoint3d* polygonPoints, 
		const AcCmEntityColor* outlineColors = NULL, const AcGiLineType* outlineTypes = NULL, 
		const AcCmEntityColor* fillColors = NULL, const AcCmTransparency* fillOpacities = NULL) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean mesh(int eval, const Adesk::UInt32 rows, const Adesk::UInt32 columns, const AcGePoint3d* pVertexList,
		const AcGiEdgeData* pEdgeData = NULL, const AcGiFaceData* pFaceData = NULL, 
		const AcGiVertexData* pVertexData = NULL, const bool bAutoGenerateNormals = true) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean shell(int eval, const Adesk::UInt32 nbVertex, const AcGePoint3d* pVertexList, const Adesk::UInt32 faceListSize, 
		const Adesk::Int32* pFaceList, const AcGiEdgeData* pEdgeData = NULL, const AcGiFaceData* pFaceData = NULL, 
		const AcGiVertexData* pVertexData = NULL, const struct resbuf* pResBuf = NULL, const bool bAutoGenerateNormals = true) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean text(int eval, const AcGePoint3d& position, const AcGeVector3d& normal, const AcGeVector3d& direction,
		const double height, const double width, const double oblique, const ACHAR* pMsg) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean text(int eval, const AcGePoint3d& position, const AcGeVector3d& normal, const AcGeVector3d& direction, 
		const ACHAR* pMsg, const Adesk::Int32 length, const Adesk::Boolean raw, const AcGiTextStyle &pTextStyle) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean xline(int eval, const AcGePoint3d& pt1, const AcGePoint3d& pt2) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean ray(int eval, const AcGePoint3d& pt1, const AcGePoint3d& pt2) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean pline(int eval, const AcDbPolyline& lwBuf, Adesk::UInt32 fromIndex = 0, Adesk::UInt32 numSegs = 0) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean draw(int eval, AcGiDrawable*p) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean image(int eval, const AcGiImageBGRA32& imageSource, const AcGePoint3d& position, 
		const AcGeVector3d& u, const AcGeVector3d& v, AcGiGeometry::TransparencyMode transparencyMode = AcGiGeometry::kTransparency8Bit) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean rowOfDots(int eval, int count, const AcGePoint3d& start, const AcGeVector3d& step) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean ellipticalArc(int eval, const AcGePoint3d& center, const AcGeVector3d& normal, double majorAxisLength, double minorAxisLength, 
		double startDegreeInRads, double endDegreeInRads, double tiltDegreeInRads, AcGiArcType arcType = kAcGiArcSimple) /*const*/
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean pushClipBoundary(int eval, AcGiClipBoundary * pBoundary) 
	{ return Adesk::kTrue; }

	virtual void popClipBoundary(int eval) { }

	virtual Adesk::Boolean worldLine(int eval, const AcGePoint3d pnts[2]) 
	{ return Adesk::kTrue; }

	virtual Adesk::Boolean polypoint(int eval, const Adesk::UInt32 nbPoints, const AcGePoint3d* pVertexList, 
		const AcGeVector3d* pNormal = NULL, const Adesk::LongPtr *pSubEntMarkers = NULL) const
	{ return Adesk::kTrue; }

#if	(ACADV_RELMAJOR >= 20)
	virtual Adesk::Boolean edge(int eval, const AcArray<AcGeCurve2d *>& edges) /*const*/
	{ return Adesk::kTrue; }
#endif
};

class IDbDwgFiler
{
public:
	virtual Acad::ErrorStatus filerStatus(int eval) /*const*/
	{ return Acad::eOk; }

	virtual AcDb::FilerType filerType(int eval) /*const*/
	{ return AcDb::kUndoFiler; }

	virtual void setFilerStatus(int eval, Acad::ErrorStatus) { }

	virtual void resetFilerStatus(int eval) { }

	virtual Acad::ErrorStatus readHardOwnershipId(int eval, AcDbHardOwnershipId* id) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeHardOwnershipId(int eval, const AcDbHardOwnershipId& id) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readSoftOwnershipId(int eval, AcDbSoftOwnershipId* id) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeSoftOwnershipId(int eval, const AcDbSoftOwnershipId& id)
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readHardPointerId(int eval, AcDbHardPointerId* id) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeHardPointerId(int eval, const AcDbHardPointerId& id) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readSoftPointerId(int eval, AcDbSoftPointerId* id) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeSoftPointerId(int eval, const AcDbSoftPointerId& id) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readInt8(int eval, Adesk::Int8 * i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeInt8(int eval, Adesk::Int8 i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readString(int eval, ACHAR ** ppstr) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeString(int eval, const ACHAR * pstr) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readString(int eval, AcString & str) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeString(int eval, const AcString & str) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readBChunk(int eval, ads_binary * ab) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeBChunk(int eval, const ads_binary& ab) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readAcDbHandle(int eval, AcDbHandle* h) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeAcDbHandle(int eval, const AcDbHandle& h) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readInt64(int eval, Adesk::Int64* i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeInt64(int eval, Adesk::Int64 i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readInt32(int eval, Adesk::Int32* i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeInt32(int eval, Adesk::Int32 i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readInt16(int eval, Adesk::Int16* i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeInt16(int eval, Adesk::Int16 i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readUInt64(int eval, Adesk::UInt64* i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeUInt64(int eval, Adesk::UInt64 i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readUInt32(int eval, Adesk::UInt32* i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeUInt32(int eval, Adesk::UInt32 i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readUInt16(int eval, Adesk::UInt16* i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeUInt16(int eval, Adesk::UInt16 i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readUInt8(int eval, Adesk::UInt8* i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeUInt8(int eval, Adesk::UInt8 i) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readBoolean(int eval, Adesk::Boolean* b) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeBoolean(int eval, Adesk::Boolean b) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readBool(int eval, bool* b) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeBool(int eval, bool b) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readDouble(int eval, double* d) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeDouble(int eval, double d) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readPoint2d(int eval, AcGePoint2d* pt) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writePoint2d(int eval, const AcGePoint2d& pt) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readPoint3d(int eval, AcGePoint3d* pt) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writePoint3d(int eval, const AcGePoint3d& pt) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readVector2d(int eval, AcGeVector2d* pt) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeVector2d(int eval, const AcGeVector2d& v) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readVector3d(int eval, AcGeVector3d* v) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeVector3d(int eval, const AcGeVector3d& v) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readScale3d(int eval, AcGeScale3d* s) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeScale3d(int eval, const AcGeScale3d& s) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus readBytes(int eval, void* p, Adesk::UIntPtr n) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus writeBytes(int eval, const void * p, Adesk::UIntPtr n) 
	{ return Acad::eOk; }

	virtual Acad::ErrorStatus seek(int eval, Adesk::Int64 nOffset, int nMethod) 
	{ return Acad::eOk; }

	virtual Adesk::Int64 tell(int eval) /*const*/
	{ return 0xffff; }

};

class IWatcherDwgOutFields
{
public:
	virtual void setReader(IDbDwgFiler* pDwgFiler) = 0;
	virtual IDbDwgFiler* getReader() = 0;

	virtual Adesk::Boolean watch(AcDbObject* pObj) = 0;
};


class IWatcherSubWorldDraw
{
public:
	virtual void setReader(IGiWorldGeometry* geom) = 0;
	virtual IGiWorldGeometry* getReader() = 0;

	virtual void setRegenType(AcGiRegenType regenType) = 0;
	virtual AcGiRegenType getRegenType() = 0;

	virtual Adesk::Boolean watch(AcDbObject* pObj) = 0;
};


class IWatcherSubViewportDraw
{
public:
	virtual void setReader(IGiViewportGeometry* geom) = 0;
	virtual IGiViewportGeometry* getReader() = 0;

	virtual void setRegenType(AcGiRegenType regenType) = 0;
	virtual AcGiRegenType getRegenType() = 0;

	virtual void watch(AcDbObject* pObj) = 0;
};

#ifdef HackARX_MODULE
#define DLLIMPEXP extern "C" __declspec(dllexport)
#else
#define DLLIMPEXP extern "C" _declspec(dllimport)
#endif


DLLIMPEXP IWatcherDwgOutFields* CreateDwgOutFieldsWatcher();
DLLIMPEXP IWatcherSubWorldDraw* CreateSubWorldDrawWatcher();
DLLIMPEXP IWatcherSubViewportDraw* CreateSubViewportDrawWatcher();

DLLIMPEXP void ReleaseDwgOutFieldsWatcher(IWatcherDwgOutFields* pWatcher);
DLLIMPEXP void ReleaseSubWorldDrawWatcher(IWatcherSubWorldDraw* pWatcher);
DLLIMPEXP void ReleaseSubViewportDrawWatcher(IWatcherSubViewportDraw* pWatcher);