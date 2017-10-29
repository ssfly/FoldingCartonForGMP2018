#pragma once

#include <qgl.h>
#include <gl\GLU.h>
#include <QMouseEvent>
#include <QGLViewer/qglviewer.h>
#include <vector>
#include <qobject.h>
#include <cv.h>
#include "mymesh.h"

#define E 0.0001
using namespace std;
using namespace cv;

static GLfloat colorSpace_[][3] = {
	{ 241. / 255., 109. / 255., 122. / 255. },{ 241. / 255., 204. / 255.,184. / 255. },{ 219. / 255., 136. / 255., 120. / 255. },
	{ 183. / 255., 210. / 255., 141. / 255. },{ 184. / 255., 241. / 255., 237. / 255. },{ 241. / 255., 241. / 255., 184. / 255. },
	{ 231. / 255., 218. / 255., 201. / 255. },{ 241. / 255., 184. / 255., 228. / 255. },{ 243. / 255., 214. / 255., 78. / 255. },
	{ 254. / 255., 249. / 255., 69. / 255. } ,{ 227. / 255., 124. / 255., 91. / 255. },{ 255. / 255., 130. / 255., 64. / 255. },
	{ 207. / 255., 136. / 255., 136. / 255. },{ 226. / 255., 158. / 255., 75. / 255. },{ 220. / 255., 255. / 255., 147. / 255. },
	{ 242. / 255., 222. / 255., 189. / 255. },{ 207. / 255., 136. / 255., 136. / 255. }, {254. / 255., 207. / 255., 69. / 255. },
	{ 184. / 255., 241. / 255., 204. / 255. },{ 254. / 255., 151. / 255., 120. / 255. }, {255. / 255., 229. / 255., 67. / 255. }
};

static int colorNum_ = 21;


class Viewer : public QGLViewer, public QObject
{
	Q_OBJECT
public:
	Viewer() : selectionmode_(NONE), type_(NONE_) { isCandidate = false; isUsed = false; needUpdateColor = true; };
	//~Viewer();

	enum SelectionMode
	{
		NONE,
		ADD,
		REMOVE
	};
	enum Type
	{
		NONE_,
		VERTEX,
		FACE,
		EDGE
	};
	virtual void draw();
	virtual void init();
	virtual void drawWithNames();
	virtual void endSelection(const QPoint &);

	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);

	void getEdgeVertex();
	void clearViewer();
	void setColorIdx(vector<int> colorIdx);
	vector<int> getColorIdx();
	void Viewer::drawFace(int id);

	MyMesh mesh;
	SelectionMode selectionmode_;
	Type type_;
	QList<int> selected_vertices;
	QList<int> selected_edges;
	QList<int> selected_faces;
	bool isCandidate;
	vector<int> mergeVertices;
	int viewIdx;
	bool isUsed;
	bool needUpdateColor;

signals:
	void isDoubleClicked(int);

private:
	vector<int> edgeVertex0;
	vector<int> edgeVertex1;

	void drawSelectionRectangle() const;
	void addIdToSelection(int id);
	void removeIdFromSelection(int id);

	void drawVertexWithName();
	void drawFaceWithName();
	void drawEdgeWithName();

	QRect rectangle_;

	void drawVertex(int id);
	void drawEdge(int id);

	void updateMeshColor();
	vector<int> faceColorIdx;

	GLUtesselator * tesser();


};

