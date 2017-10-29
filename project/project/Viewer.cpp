#include "Viewer.h"





void Viewer::getEdgeVertex()
{
	int n = mesh.n_edges();
	vector<int> vtmp0(n, -1);
	vector<int> vtmp1(n, -1);
	//initVector(edgeVertex0, n);
	//initVector(edgeVertex1, n);
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); v_it++)
	{
		int vidx = (*v_it).idx();
		for (MyMesh::VertexEdgeIter ve_it = mesh.ve_iter(*v_it); ve_it.is_valid(); ve_it++)
		{
			int eidx = (*ve_it).idx();
			if (vtmp0[eidx] == -1)
				vtmp0[eidx] = vidx;
			else vtmp1[eidx] = vidx;
		}
	}
	edgeVertex0 = vtmp0;
	edgeVertex1 = vtmp1;
	
	faceColorIdx.resize(mesh.n_faces());
	for (int i = 0; i < mesh.n_faces(); i++)
		faceColorIdx[i] = i;
	if (needUpdateColor) updateMeshColor();
}

void Viewer::updateMeshColor()
{
	for (MyMesh::FaceIter f_it0 = mesh.faces_begin(); (f_it0 + 1) != mesh.faces_end(); f_it0++)
	{
		MyMesh::FaceVertexIter fv0 = mesh.fv_iter(*f_it0);
		for (MyMesh::FaceIter f_it1 = mesh.faces_begin() + 1; f_it1 != mesh.faces_end(); f_it1++)
		{
			bool areSame = true;
			for (MyMesh::FaceVertexIter fv = mesh.fv_iter(*f_it1); fv.is_valid(); fv++)
			{
				float dist_x = (mesh.point(*fv)[0] - mesh.point(*fv0)[0]) * mesh.normal(*f_it0)[0];
				float dist_y = (mesh.point(*fv)[1] - mesh.point(*fv0)[1]) * mesh.normal(*f_it0)[1];
				float dist_z = (mesh.point(*fv)[2] - mesh.point(*fv0)[2]) * mesh.normal(*f_it0)[2];
				float dist = dist_x + dist_y + dist_z;
				if (abs(dist) > E)
				{
					areSame = false;
					continue;
				}
			}
			if (areSame && faceColorIdx[(*f_it1).idx()] != faceColorIdx[(*f_it0).idx()])
			{
				//mesh.set_color((*f_it1), MyMesh::Color(mesh.color(*f_it0)[0], mesh.color(*f_it0)[1], mesh.color(*f_it0)[2]));
				faceColorIdx[(*f_it1).idx()] = faceColorIdx[(*f_it0).idx()];
			}
		}
	}
}

void Viewer::setColorIdx(vector<int> colorIdx)
{
	faceColorIdx.clear();
	faceColorIdx = colorIdx;
}

vector<int> Viewer::getColorIdx()
{
	return faceColorIdx;
}
void Viewer::drawVertex(int id)
{
	MyMesh::VertexHandle vh(id);
	Vec3d pos = { mesh.point(vh)[0],  mesh.point(vh)[1] ,mesh.point(vh)[2] };
	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]);
	GLUquadric *quad = gluNewQuadric();
	gluSphere(quad, 4., 4., 4.);
	glPopMatrix();
}
void Viewer::drawEdge(int id)
{
	MyMesh::EdgeHandle eh(id);
	//MyMesh::HalfedgeHandle ehh = mesh.halfedge_handle(eh, 0);
	//MyMesh::VertexHandle vh1 = mesh.from_vertex_handle(ehh);
	//MyMesh::VertexHandle vh2 = mesh.to_vertex_handle(ehh);
	MyMesh::VertexHandle vh1(edgeVertex0[id]);
	MyMesh::VertexHandle vh2(edgeVertex1[id]);
	glLineWidth(5);
	glBegin(GL_LINE_STRIP);
	glVertex3f(mesh.point(vh1)[0], mesh.point(vh1)[1], mesh.point(vh1)[2]);
	glVertex3f(mesh.point(vh2)[0], mesh.point(vh2)[1], mesh.point(vh2)[2]);
	glEnd();
}
void Viewer::drawFace(int id)
{
	MyMesh::FaceHandle fh(id);
	//mesh.set_color(fh, MyMesh::Color(0,0,0));

	glBegin(GL_POLYGON);
	for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(fh); fv_it.is_valid(); fv_it++)
	{
		glVertex3f(mesh.point(*fv_it)[0], mesh.point(*fv_it)[1], mesh.point(*fv_it)[2]);
	}
	glEnd();
}
void Viewer::clearViewer()
{
	mesh.clear();
	edgeVertex0.clear();
	edgeVertex1.clear();
	mergeVertices.clear();
	//selectionmode_ = NONE;
	//type_ = NONE_;
	selected_edges.clear();
	selected_vertices.clear();
	isCandidate = false;
	isUsed = false;
}

GLUtesselator * Viewer::tesser()
{
	GLUtesselator * tess;
	tess = gluNewTess();
	gluTessCallback(tess, GLU_TESS_BEGIN, (void(_stdcall *)()) &glBegin);
	gluTessCallback(tess, GLU_TESS_VERTEX, (void(_stdcall *)()) &glVertex3dv);
	gluTessCallback(tess, GLU_TESS_END, (void(_stdcall *)()) &glEnd);
	return tess;
}

void Viewer::draw()
{
	glDisable(GL_LIGHTING);
	int colorIdx = 0;

	//凹平面赋颜色
	//由于gluTessBeginContour循环内不能有局部变量，因此需要在外赋值

	vector<vector<vector<GLdouble>>> v_data;
	vector<vector<GLdouble>> tmpList;
	vector<GLdouble> tmp(3,0);

	vector<int> fvSize;
	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
	{
		int count = 0;
		for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
			count++;
		fvSize.push_back(count);
	}
	
	for (int i = 0; i < fvSize.size(); i++)
	{
		for (int j = 0; j < fvSize[i]; j++)
			tmpList.push_back(tmp);
			
		v_data.push_back(tmpList);
		tmpList.clear();
	}


	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
	{
		int count = 0;
		for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
		{
			v_data[(*f_it).idx()][count][0] = mesh.point(*fv_it)[0];
			v_data[(*f_it).idx()][count][1] = mesh.point(*fv_it)[1];
			v_data[(*f_it).idx()][count][2] = mesh.point(*fv_it)[2];
			count++;
		}
	}


	glClear(GL_STENCIL_BUFFER_BIT);
	GLUtesselator* tess = tesser();
	if (!tess) return;
	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
	{

		//glColor3f(colorSpace_[colorIdx % colorNum_][0], colorSpace_[colorIdx % colorNum_][1], colorSpace_[colorIdx % colorNum_][2]);
		//colorIdx++;
		//glColor3f(mesh.color(*f_it)[0] / 255., mesh.color(*f_it)[1] / 255., mesh.color(*f_it)[2] / 255.);
		glColor3f(colorSpace_[faceColorIdx[(*f_it).idx()] % colorNum_][0], colorSpace_[faceColorIdx[(*f_it).idx()] % colorNum_][1], colorSpace_[faceColorIdx[(*f_it).idx()] % colorNum_][2]);

		gluTessBeginPolygon(tess, NULL);
		gluTessBeginContour(tess);

		for (int i = 0; i<fvSize[(*f_it).idx()]; i++)
			gluTessVertex(tess, &v_data[(*f_it).idx()][i][0], &v_data[(*f_it).idx()][i][0]);


		gluTessEndContour(tess);
		gluTessEndPolygon(tess);
	}


	v_data.clear();

	for (MyMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); e_it++)
	{
		//drawEdge((*e_it).idx());
		MyMesh::VertexHandle vh1(edgeVertex0[(*e_it).idx()]);
		MyMesh::VertexHandle vh2(edgeVertex1[(*e_it).idx()]);
		glLineWidth(2);
		glBegin(GL_LINE_STRIP);
		glColor3f(0.f, 0.f, 0.f);
		glVertex3f(mesh.point(vh1)[0], mesh.point(vh1)[1], mesh.point(vh1)[2]);
		glVertex3f(mesh.point(vh2)[0], mesh.point(vh2)[1], mesh.point(vh2)[2]);
		glEnd();
	}
	if (isCandidate)
	{
		glColor3f(0.f, 1.f, 0.f);
		drawVertex(mergeVertices[0]);
		MyMesh::VertexHandle vh1(mergeVertices[0]);
		for (int i = 1; i < mergeVertices.size(); i++)
		{
			drawVertex(mergeVertices[i]);
			glColor3f(0.f, 1.f, 0.f);
			MyMesh::VertexHandle vh2(mergeVertices[i]);
			glLineStipple(2, 0x5555);
			glLineWidth(3);
			glEnable(GL_LINE_STIPPLE);
			glBegin(GL_LINE_STRIP);
			glVertex3f(mesh.point(vh1)[0], mesh.point(vh1)[1], mesh.point(vh1)[2]);
			glVertex3f(mesh.point(vh2)[0], mesh.point(vh2)[1], mesh.point(vh2)[2]);
			glEnd();
			glDisable(GL_LINE_STIPPLE);
		}
	}
	switch (type_)
	{
	case VERTEX:
	{
		glColor3f(1.f, 0.f, 0.f);
		//for (QList<int>::const_iterator it = selected_vertices.begin(), end = selected_vertices.end(); it != end; it++)
		//{
		//	drawVertex(selected_vertices.at(*it));
		//}
		for (int i = 0; i < selected_vertices.size(); i++)
		{
			drawVertex(selected_vertices.at(i));
		}
		glColor3f(0.f, 1.f, 0.f);
		for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); v_it++)
		{
			if (!selected_vertices.contains((*v_it).idx()))
				drawVertex((*v_it).idx());
		}
		if (selectionmode_ != NONE) drawSelectionRectangle();
		break;
	}
	case EDGE:
	{
		glColor3f(1.f, 0.f, 0.f);
		for (int i = 0; i < selected_edges.size(); i++)
		{
			drawEdge(selected_edges.at(i));
		}
		glColor3f(0.f, 1.f, 0.f);
		int n = mesh.n_edges();
		for (MyMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); e_it++)
		{
			if (!selected_edges.contains((*e_it).idx()))
			{
				drawEdge((*e_it).idx());
			}
		}
		if (selectionmode_ != NONE) drawSelectionRectangle();
		break;
	}
	case FACE:
		//glColor4f(0.f, 0.f, 0.f, 1.f);

		for (int i = 0; i < selected_faces.size(); i++)
		{
			MyMesh::FaceHandle fh(selected_faces.at(i));
			glColor3f(1.f, 0.f, 0.f);
			for (MyMesh::FaceEdgeIter fe_it = mesh.fe_iter(fh); fe_it.is_valid(); fe_it++)
			{
				drawEdge((*fe_it).idx());
			}
			//mesh.set_color(fh, MyMesh::Color(0, 0, 0));
			//drawFace(selected_faces.at(i));
		}
		if (selectionmode_ != NONE) drawSelectionRectangle();
		break;
	default: break;
	}

}

void Viewer::mousePressEvent(QMouseEvent *e)
{
	if (type_ != NONE_)
	{
		rectangle_ = QRect(e->pos(), e->pos());
		if ((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::ShiftModifier))
		{
			selectionmode_ = ADD;
		}
		else
			if ((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::AltModifier))
			{
				selectionmode_ = REMOVE;
			}
			else
			{
				QGLViewer::mousePressEvent(e);
			}
	}
	else QGLViewer::mousePressEvent(e);
}

void Viewer::mouseMoveEvent(QMouseEvent *e)
{
	if ((type_ != NONE_) && (selectionmode_ != NONE))
	{
		rectangle_.setBottomRight(e->pos());
		update();
	}
	else
		QGLViewer::mouseMoveEvent(e);
}

void Viewer::mouseReleaseEvent(QMouseEvent *e)
{
	if (selectionmode_ != NONE)
	{
		rectangle_ = rectangle_.normalized();
		setSelectRegionWidth(rectangle_.width());
		setSelectRegionHeight(rectangle_.height());
		select(rectangle_.center());
		update();
	}
	else QGLViewer::mouseReleaseEvent(e);
}

void Viewer::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (isCandidate)
	{
		emit isDoubleClicked(viewIdx);
	}
	else QGLViewer::mouseDoubleClickEvent(e);
}

void Viewer::drawWithNames()
{
	switch (type_)
	{
	case VERTEX:
		drawVertexWithName();
		break;
	case FACE:
		drawFaceWithName();
		break;
	case EDGE:
		drawEdgeWithName();
		break;
	default:
		break;
	}
}

void Viewer::drawVertexWithName()
{
	for (int i = 0; i < mesh.n_vertices(); i++)
	{
		glPushName(i);
		drawVertex(i);
		glPopName();
	}
}
void Viewer::drawFaceWithName()
{
	for (int i = 0; i < mesh.n_faces(); i++)
	{
		glPushName(i);
		drawFace(i);
		glPopName();
	}
}

void Viewer::drawEdgeWithName()
{
	for (int i = 0; i < mesh.n_edges(); i++)
	{
		glPushName(i);
		drawEdge(i);
		glPopName();
	}
}

void Viewer::endSelection(const QPoint &)
{
	glFlush();
	GLint nbHits = glRenderMode(GL_RENDER);
	if (nbHits > 0)
	{
		for (int i = 0; i < nbHits; i++)
			switch (selectionmode_)
			{
			case ADD:
				addIdToSelection((selectBuffer())[4 * i + 3]);
				break;
			case REMOVE:
				removeIdFromSelection((selectBuffer())[4 * i + 3]);
				break;
			default:
				break;
			}
	}
	selectionmode_ = NONE;
}

void Viewer::addIdToSelection(int id)
{
	switch (type_)
	{
	case VERTEX:
	{
		if (!selected_vertices.contains(id))
			selected_vertices.push_back(id);
		break;
	}
	case EDGE:
	{
		if (!selected_edges.contains(id))
			selected_edges.push_back(id);
		break;
	}
	case FACE:
	{
		if (!selected_faces.contains(id))
			selected_faces.push_back(id);
		break;
	}
	default:
		break;
	}

}

void Viewer::removeIdFromSelection(int id)
{
	switch (type_)
	{
	case Viewer::NONE_:
		break;
	case Viewer::VERTEX:
		selected_vertices.removeAll(id);
		break;
	case Viewer::FACE:
	{
		selected_faces.removeAll(id);
		//MyMesh::FaceHandle fh(id);
		//mesh.set_color(fh, MyMesh::Color(colorSpace_[id % colorNum_][0] * 255, colorSpace_[id % colorNum_][1] * 255, colorSpace_[id % colorNum_][2] * 255));
		break;
	}
	case Viewer::EDGE:
		selected_edges.removeAll(id);
		break;
	default:
		break;
	}

}

void Viewer::drawSelectionRectangle() const
{
	startScreenCoordinatesSystem();
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	glColor4f(0.0, 0.0, 0.3f, 0.3f);
	glBegin(GL_QUADS);
	glVertex2i(rectangle_.left(), rectangle_.top());
	glVertex2i(rectangle_.right(), rectangle_.top());
	glVertex2i(rectangle_.right(), rectangle_.bottom());
	glVertex2i(rectangle_.left(), rectangle_.bottom());
	glEnd();

	glLineWidth(2.0);
	glColor4f(0.4f, 0.4f, 0.5f, 0.5f);
	glBegin(GL_LINE_LOOP);
	glVertex2i(rectangle_.left(), rectangle_.top());
	glVertex2i(rectangle_.right(), rectangle_.top());
	glVertex2i(rectangle_.right(), rectangle_.bottom());
	glVertex2i(rectangle_.left(), rectangle_.bottom());
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	stopScreenCoordinatesSystem();
}

void Viewer::init()
{
	// Restore previous viewer state.
	setBackgroundColor(Qt::white);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	//	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_LIGHTING);
	camera()->setFieldOfView((15. / 180.)*3.1415926);
	restoreStateFromFile();
	//  help();
}

