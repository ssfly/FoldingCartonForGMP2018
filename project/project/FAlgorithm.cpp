#include "FAlgorithm.h"
#include<fstream>
#include<unordered_set>
#include <qdebug.h>

FAlgorithm::FAlgorithm()
{
	LoadParameters();
}


FAlgorithm::~FAlgorithm()
{
}

void FAlgorithm::LoadParameters()
{
	ifstream para("./data/parameters.txt");
	string type;
	double t_double;
	int t_int;
	string t_str;
	while (para)
	{
		para >> type;
		if (type == "MyPi")
		{
			para >> t_double;
			g_parameters.MyPi = t_double;
		}
		else if (type == "InputFilePath")
		{
			para >> t_str;
			g_parameters.InputFilePath = t_str;
		}
		else if (type == "MyEpsilon")
		{
			para >> t_double;
			g_parameters.MyEpsilon = t_double;
		}
		else if (type == "MyInfinity")
		{
			para >> t_int;
			g_parameters.MyInfinity = t_int;
		}
		else if (type == "ResolutionMultipe")
		{
			para >> t_int;
			g_parameters.ResolutionMultipe = t_int;
		}
		else if (type == "ThresholdForSeparateLines")
		{
			para >> t_double;
			g_parameters.ThresholdForSeparateLines = t_double;
		}
	}

	para.close();
}


float FAlgorithm::computeDistance(MyMesh m, MyMesh::VertexHandle vh1, MyMesh::VertexHandle vh2)
{
	return sqrtf((m.point(vh1)[0] - m.point(vh2)[0]) * (m.point(vh1)[0] - m.point(vh2)[0]) +
		(m.point(vh1)[1] - m.point(vh2)[1]) * (m.point(vh1)[1] - m.point(vh2)[1]) +
		(m.point(vh1)[2] - m.point(vh2)[2]) * (m.point(vh1)[2] - m.point(vh2)[2]));
}

void FAlgorithm::InitializeShapeConstrain(MyMesh mesh)
{
	
	//ShapeOp::Matrix3X p;

	const int vSize = mesh.n_vertices();
	p.resize(3, vSize);
	

	//输入顶点
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		p(0, (*v_it).idx()) = mesh.point((*v_it))[0];
		p(1, (*v_it).idx()) = mesh.point((*v_it))[1];
		p(2, (*v_it).idx()) = mesh.point((*v_it))[2];
	}

	//add basic constrains	
	s.setPoints(p);
	ShapeOp::Scalar weight = 1.0;

	/*ofstream fout("stdout.txt");
	fout << s.getPoints();
	fout.close();*/

	//coplane constrain
	//for each plane, add Plane Constrain
	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
	{
		std::vector<int> id_vector;
		for (MyMesh::FaceVertexIter fv_it = mesh.fv_begin(*f_it); fv_it != mesh.fv_end(*f_it); fv_it++)
			id_vector.push_back((*fv_it).idx());
		//cout << s.getPoints() << endl;
		auto c = std::make_shared<ShapeOp::PlaneConstraint>(id_vector, weight, s.getPoints());
		s.addConstraint(c);
	}

	//plane constrain
	/*for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
		for (MyMesh::FaceVertexIter fv_it = mesh.fv_begin(*f_it); fv_it != mesh.fv_end(*f_it); fv_it++)
		{
			unordered_set<MyMesh::VertexHandle> sv;
			for (MyMesh::VertexVertexIter vv_it = mesh.vv_begin(*fv_it); vv_it != mesh.vv_end(*fv_it); vv_it++)
				sv.insert(*vv_it);
			for (MyMesh::FaceVertexIter fvv_it = mesh.fv_begin(*f_it); fvv_it != mesh.fv_end(*f_it); fvv_it++)
			{

				if (fvv_it != fv_it && sv.find(*fvv_it) == sv.end())
				{
					std::vector<int> id_vector;
					id_vector.push_back((*fv_it).idx());
					id_vector.push_back((*fvv_it).idx());
					auto c = std::make_shared<ShapeOp::SimilarityConstraint>(id_vector, weight, s.getPoints(), false, true, false);
					s.addConstraint(c);
				}
			}
		}*/
			
	
	

	//edge constrain and plane constrain 2
	//Similarity constraint
	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
		for (MyMesh::FaceVertexIter fv_it = mesh.fv_begin(*f_it); fv_it != mesh.fv_end(*f_it); fv_it++)
			for (MyMesh::FaceVertexIter fvv_it = mesh.fv_begin(*f_it); fvv_it != mesh.fv_end(*f_it); fvv_it++)
			{
				if (fvv_it != fv_it)
				{
					std::vector<int> id_vector;
					id_vector.push_back((*fv_it).idx());
					id_vector.push_back((*fvv_it).idx());
					auto c = std::make_shared<ShapeOp::SimilarityConstraint>(id_vector, weight, s.getPoints(), false, true, false);
					s.addConstraint(c);
				}
			}
}

MyMesh FAlgorithm::ShapeOptimization(MyMesh mesh, vector<MyMesh::VertexHandle> vhlist)
{
	ShapeOp::Scalar weight = 1.0;
	for (int i = 0; i < vhlist.size(); i++)
	{
		int j = (i + 1) % vhlist.size();
		std::vector<int> id_vector;
		id_vector.push_back(vhlist[i].idx());
		id_vector.push_back(vhlist[j].idx());
		auto c = std::make_shared<ShapeOp::EdgeStrainConstraint>(id_vector, weight, s.getPoints());
		c->setEdgeLength(0.00001);
		s.addConstraint(c);
	}

	//soft constrain: modified point constrain 2
	//Closeness constrain
	ShapeOp::Scalar soft_weight = 0.001;

	std::vector<OpenMesh::VertexHandle> vHandle;
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
		vHandle.push_back(*v_it);

	std::vector<bool> flag(vHandle.size(), 1);
	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
	{
		bool hasVertex = 0;
		for (MyMesh::FaceVertexIter fvv_it = mesh.fv_begin(*f_it); fvv_it != mesh.fv_end(*f_it); fvv_it++)
			//若需要合并的点在面内
			for (int i = 0; i < vhlist.size(); i++)
				if (vhlist[i].idx() == (*fvv_it).idx())
				{
					hasVertex = 1;
					break;   //只能跳出一重循环
				}

		if (hasVertex)
			for (MyMesh::FaceVertexIter fvv_it = mesh.fv_begin(*f_it); fvv_it != mesh.fv_end(*f_it); fvv_it++)
				flag[(*fvv_it).idx()] = 0;

	}

	for (int i = 0; i < flag.size(); i++)
	{
		if (flag[i])
		{
			//std::cout <<"Need not move : "<< i << std::endl;
			std::vector<int> id_vector;
			id_vector.push_back(vHandle[i].idx());
			auto c = std::make_shared<ShapeOp::ClosenessConstraint>(id_vector, soft_weight, s.getPoints());
			c->setPosition(ShapeOp::Vector3(mesh.point(vHandle[i])[0], mesh.point(vHandle[i])[1], mesh.point(vHandle[i])[2]));
			s.addConstraint(c);
		}
	}



	s.initialize();
	s.solve(2000);
	p = s.getPoints();

	//output final points
	/*std::cout << "Output points:" << std::endl;
	for (int i = 0; i < vSize; ++i) {
	std::cout << "Point " << i << " : ( ";
	ShapeOp::Vector3 current_pt = p.col(i);
	std::cout << std::scientific << current_pt.transpose();
	std::cout << " )" << std::endl;
	}*/

	MyMesh cur_mesh = mesh;
	cur_mesh.request_face_normals();
	cur_mesh.request_face_colors();

	//m_cur_mesh 顶点赋值
	for (MyMesh::VertexIter v_it = cur_mesh.vertices_begin(); v_it != cur_mesh.vertices_end(); ++v_it)
	{
		OpenMesh::Vec3f coor = OpenMesh::Vec3f(float(p(0, (*v_it).idx())), float(p(1, (*v_it).idx())), float(p(2, (*v_it).idx())));
		cur_mesh.set_point((*v_it), coor);
	}




	//output m_cur_mesh points
	/*for (MyMesh::VertexIter v_it = m_cur_mesh.vertices_begin(); v_it != m_cur_mesh.vertices_end(); ++v_it)
	std::cout << (*v_it).idx() << "  " << m_cur_mesh.point((*v_it))[0] << "  " << m_cur_mesh.point((*v_it))[1] << "  " << m_cur_mesh.point((*v_it))[2] << std::endl;*/

	//m_cur_mesh.update_normals();
	cur_mesh.update_face_normals();
	return cur_mesh;

}

MyMesh FAlgorithm::ShapeOptimization(MyMesh mesh, vector<MyMesh::FaceHandle> fhlist)
{
	//select planes, and optimize these planes into one 
	ShapeOp::Scalar weight = 1.0;
	unordered_set<int> id_tmp; //不加入重复id
	for (int i = 0; i < fhlist.size(); i++)
	{
		for (MyMesh::FaceVertexIter fv_it = mesh.fv_begin(fhlist[i]); fv_it != mesh.fv_end(fhlist[i]); fv_it++)
		{
			if (id_tmp.find((*fv_it).idx()) == id_tmp.end())
				id_tmp.insert((*fv_it).idx());
		}
	}
	vector<int> id_vector(id_tmp.begin(),id_tmp.end());
	auto c = std::make_shared<ShapeOp::PlaneConstraint>(id_vector, weight, s.getPoints());
	s.addConstraint(c);

	s.initialize();
	s.solve(2000);
	p = s.getPoints();

	MyMesh cur_mesh = mesh;
	cur_mesh.request_face_normals();
	cur_mesh.request_face_colors();

	//m_cur_mesh 顶点赋值
	for (MyMesh::VertexIter v_it = cur_mesh.vertices_begin(); v_it != cur_mesh.vertices_end(); ++v_it)
	{
		OpenMesh::Vec3f coor = OpenMesh::Vec3f(float(p(0, (*v_it).idx())), float(p(1, (*v_it).idx())), float(p(2, (*v_it).idx())));
		cur_mesh.set_point((*v_it), coor);
	}

	//m_cur_mesh.update_normals();
	cur_mesh.update_face_normals();
	return cur_mesh;


}

vector<MyMesh::VertexHandle> FAlgorithm::symmetricVertex(MyMesh mesh,MyMesh::VertexHandle & vh)
{
	std::vector<MyMesh::VertexHandle> res;

	double thres_diff = 1;  //对称顶点邻接边长度的和的差异设为1
	set<double> s_edge;

	set<double> t_edge;
	for (MyMesh::VertexEdgeIter veh = mesh.ve_begin(vh); veh != mesh.ve_end(vh); veh++)
		s_edge.insert(mesh.calc_edge_length(*veh));

	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); v_it++)
	{
		if (vh != (*v_it))
		{
			for (MyMesh::VertexEdgeIter veh = mesh.ve_begin(*v_it); veh != mesh.ve_end(*v_it); veh++)
				t_edge.insert(mesh.calc_edge_length(*veh));

			//判断是否每条边都对应相等
			int isEqual = 1;
			if (s_edge.size() == t_edge.size())
			{
				set<double>::iterator t_it = t_edge.begin();
				for (set<double>::iterator s_it = s_edge.begin(); s_it != s_edge.end(); s_it++)
				{
					if (fabs(*s_it - *t_it) > thres_diff)
					{
						isEqual = 0;
						break;
					}
					t_it++;
				}
				if (isEqual == 1)
					res.push_back(*v_it);
			}
			t_edge.clear();
		}
	}

	return res;
}

bool compare(MyMesh::VertexHandle v1h, MyMesh::VertexHandle v2h)
{
	return v1h.idx() < v2h.idx();
}

vector<vector<MyMesh::VertexHandle>> FAlgorithm::symmetricVertexPair(MyMesh mesh, MyMesh::VertexHandle & v1h, MyMesh::VertexHandle & v2h)
{
	std::set<std::vector<MyMesh::VertexHandle>> res;
	std::vector<MyMesh::VertexHandle> origin, temp, s1, s2;
	s1 = symmetricVertex(mesh,v1h);
	s2 = symmetricVertex(mesh,v2h);
	origin.push_back(v1h);
	origin.push_back(v2h);
	//std::sort(origin.begin(), origin.end(), compare);
	res.insert(origin);

	float dis_o = computeDistance(mesh, v1h, v2h);
	for (int i = 0; i<s1.size(); i++)
		for (int j = 0; j < s2.size(); j++)
		{
			float dis_t = computeDistance(mesh, s1[i], s2[j]);
			if (fabs(dis_o - dis_t) < 2)  //threshold for 点对之间的对称性的阈值
			{
				temp.push_back(s1[i]);
				temp.push_back(s2[j]);
				//std::sort(temp.begin(), temp.end(), compare);
				// 查找以前是否有过对应pair的存储
				std::set<std::vector<MyMesh::VertexHandle>>::iterator s_it = res.find(temp);
				if (s_it == res.end())
					res.insert(temp);
				temp.clear();
			}
		}
	res.erase(origin);

	// print result
	vector<vector<MyMesh::VertexHandle>> result;
	for (std::set<std::vector<MyMesh::VertexHandle>>::iterator s_it = res.begin(); s_it != res.end(); s_it++)
		result.push_back(*s_it);

	return result;
}

vector<vector<MyMesh::VertexHandle>> FAlgorithm::symmetryDetection(MyMesh m, vector<MyMesh::VertexHandle> vhlist)
{
	vector<vector<MyMesh::VertexHandle>> res;
	vector<vector<MyMesh::VertexHandle>> next_pair;

	if (vhlist.size() >= 2)
	{
		res = symmetricVertexPair(m, vhlist[0], vhlist[1]);
		//按一个循环来检测前一对和后一对点之间有重复顶点的对称点集
		for (int i = 1; i < vhlist.size(); i++)
		{
			int j = (i + 1) % vhlist.size();
			next_pair = symmetricVertexPair(m, vhlist[i], vhlist[j]);
			for (int k = 0; k < res.size(); k++)
				if (res[k].size() == i + 1)   //若已有的点对长度为i+1，
				{
					for (int q = 0; q < next_pair.size(); q++)
					{
						if (res[k][1] == next_pair[q][0])
							res[k].push_back(next_pair[q][1]);
						else if(res[k][1] == next_pair[q][1])
							res[k].push_back(next_pair[q][0]);

					}
				}
		}

		for (vector<vector<MyMesh::VertexHandle>>::iterator it = res.begin(); it != res.end();)
		{
			if ((*it).size() != vhlist.size())
				res.erase(it);
			else
				it++;
		}
	}
	else if (vhlist.size() == 1)
	{
		vector<MyMesh::VertexHandle> tmpR = symmetricVertex(m, vhlist[0]);
		res.push_back(tmpR);
	}
		
	

	return res;
}

vector<vector<MyMesh::VertexHandle>> FAlgorithm::autoDetectionbySameVertex(MyMesh m)
{
	//由一个顶点延伸出来的两条不在同一个平面，但是长度相等的边很有可能被合并

	// 计算出每条边的相邻两个面的id
	vector<int> m_f1_idx;
	vector<int> m_f2_idx;
	vector<MyMesh::FaceHandle> m_fhlist;
	for (int i = 0; i < m.n_edges(); i++)
	{
		m_f1_idx.push_back(-1);
		m_f2_idx.push_back(-1);
	}

	for (MyMesh::FaceIter f_it = m.faces_begin(); f_it != m.faces_end(); ++f_it)
	{
		m_fhlist.push_back(*f_it);
		for (MyMesh::FaceEdgeIter e_it = m.fe_begin(*f_it); e_it != m.fe_end(*f_it); ++e_it)
		{
			if (-1 == m_f1_idx[(*e_it).idx()])
				m_f1_idx[(*e_it).idx()] = (*f_it).idx();
			else if (((*f_it).idx() > m_f1_idx[(*e_it).idx()]) && (-1 == m_f2_idx[(*e_it).idx()]))
				m_f2_idx[(*e_it).idx()] = (*f_it).idx();
			else if (((*f_it).idx() < m_f1_idx[(*e_it).idx()]) && (-1 == m_f2_idx[(*e_it).idx()]))
			{
				m_f2_idx[(*e_it).idx()] = m_f1_idx[(*e_it).idx()];
				m_f1_idx[(*e_it).idx()] = (*f_it).idx();
			}
			else
				std::cerr << "More than two faces for one edge ? some thing is wrong!" << endl;
		}
	}


	vector<vector<MyMesh::VertexHandle>> res;
	vector<MyMesh::VertexHandle> tmp;
	for (MyMesh::VertexIter v_it = m.vertices_begin(); v_it != m.vertices_end(); v_it++)
	{
		for (MyMesh::VertexIHalfedgeIter vih_it = m.vih_begin(*v_it); vih_it != m.vih_end(*v_it); vih_it++)
		{
			for (MyMesh::VertexOHalfedgeIter voh_it = m.voh_begin(*v_it); voh_it != m.voh_end(*v_it); voh_it++)
			{
				if (m.opposite_halfedge_handle(*vih_it) != (*voh_it) && m.next_halfedge_handle(*vih_it) != (*voh_it)
					&& m.opposite_halfedge_handle(*vih_it) != m.next_halfedge_handle(m.opposite_halfedge_handle(*voh_it))//保证两条halfedge不相反, 且不在同一个平面上
					&& m.next_halfedge_handle(m.opposite_halfedge_handle(m.next_halfedge_handle(*vih_it)))!= *voh_it)//如何保证face不相邻  
					if (m.calc_edge_length(*vih_it) == m.calc_edge_length(*voh_it))  //若两条线段长度相等
					{
						MyMesh::VertexHandle v1 = m.from_vertex_handle(*vih_it);
						MyMesh::VertexHandle v2 = m.to_vertex_handle(*voh_it);
						if (computeDistance(m, v1, v2) > 2)   //若两点距离较远，则需要合并
						{
							tmp.push_back(v1);
							tmp.push_back(v2);
							res.push_back(tmp);
							tmp.clear();
							qDebug() << m.from_vertex_handle(*vih_it).idx() << "  " << m.to_vertex_handle(*vih_it).idx() << endl;
							qDebug() << m.from_vertex_handle(m.next_halfedge_handle(*vih_it)).idx() << m.to_vertex_handle(m.next_halfedge_handle(*vih_it)).idx() << endl;
							qDebug() << m.from_vertex_handle(*voh_it).idx() << "  " << m.to_vertex_handle(*voh_it).idx() << endl;
						}
						
					}

			}
		}

	}



	return res;
}

vector<std::vector<MyMesh::VertexHandle>> FAlgorithm::autoDetection(MyMesh mesh)
{
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); v_it++)
		m_vertices.push_back(*v_it);

	//detect points that need to be joint
	float thres_joint_max = 60;
	float thres_joint_min = 5;
	float thres_diff = 1;
	float thres_edge_min = 20;  //找到的可以合起来的边长应大于20
	int class_v = 0;
	std::vector<std::vector<MyMesh::VertexHandle>> res;
	vector<int> flag(mesh.n_vertices(), 0); // flag for which pair the vertices in

	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); f_it++)
	{
		for (MyMesh::FaceVertexIter fv_it = mesh.fv_begin(*f_it); fv_it != mesh.fv_end(*f_it); fv_it++)
		{
			if (0 == flag[(*fv_it).idx()])
			{
				for (MyMesh::FaceIter ff_it = mesh.faces_begin(); ff_it != mesh.faces_end(); ff_it++)
				{
					if ((*ff_it).idx() != (*f_it).idx())
					{
						int isAdjFace = 0; //判断是否为相邻面
						for (MyMesh::FaceFaceIter adjf_id = mesh.ff_begin(*f_it); adjf_id != mesh.ff_end(*f_it); adjf_id++)
							if ((*adjf_id).idx() == (*ff_it).idx())
							{
								isAdjFace = 1;
								break;
							}

						if (isAdjFace == 1)
							break;

						//如果该平面不相邻
						for (MyMesh::FaceVertexIter ffv_it = mesh.fv_begin(*ff_it); ffv_it != mesh.fv_end(*ff_it); ffv_it++)
						{
							float dis = computeDistance(mesh, *fv_it, *ffv_it);
							if (dis < thres_joint_max && dis > thres_joint_min)
							{
								//判断两个顶点是否邻接不同边，且长度相等
								int found = 0;
								for (MyMesh::VertexEdgeIter veh = mesh.ve_begin(*fv_it); veh != mesh.ve_end(*fv_it); veh++)
								{
									for (MyMesh::VertexEdgeIter vveh = mesh.ve_begin(*ffv_it); vveh != mesh.ve_end(*ffv_it); vveh++)
									{
										if ((*veh).idx() != (*vveh).idx())
										{
											float len1 = mesh.calc_edge_length(*veh);
											float len2 = mesh.calc_edge_length(*vveh);
											if (std::fabs(len1 - len2) < thres_diff && len1 > thres_edge_min)
											{
												//若找到的顶点没有分类过
												found = 1;
												if (flag[(*ffv_it).idx()] == 0)
												{
													if (flag[(*fv_it).idx()] == 0)
													{
														flag[(*ffv_it).idx()] = ++class_v;
														flag[(*fv_it).idx()] = flag[(*ffv_it).idx()];
														//cout << class_v << ":   " << (*ffv_it).idx() << "   " << (*fv_it).idx() << endl;
													}
													else
														flag[(*ffv_it).idx()] = flag[(*fv_it).idx()];

												}
												else
												{
													if (flag[(*fv_it).idx()] == 0)
														flag[(*fv_it).idx()] = flag[(*ffv_it).idx()];
													else if (flag[(*fv_it).idx()] != flag[(*ffv_it).idx()])
														cerr << "Conflict between vertex classes !" << endl;
												}

												cout << flag[(*fv_it).idx()] << "   " << (*fv_it).idx() << "       " << (*ffv_it).idx() << "    " << (*veh).idx() << "    " << (*vveh).idx() << endl;

												break;
											}
										}

									}
									if (found == 1)
										break;
								}
							}
						}
					}

				}

			}
		}
	}


	/*for (int i = 1; i <= class_v; i++)
	{
		cout << "Class " << i << " :  ";
		for (int j = 0; j < flag.size(); j++)
		{
			if (flag[j] == i)
				cout << j << "    ";
		}
		cout << endl;
	}*/

	//output to res
	vector<MyMesh::VertexHandle> tmpList;
	for (int i = 1; i <= class_v; i++)
	{
		for (int j = 0; j < flag.size(); j++)
		{
			if (flag[j] == i)
				tmpList.push_back(m_vertices[j]);
		}
		res.push_back(tmpList);
		tmpList.clear();
	}

	//加上autoDetectionbySameVertex的计算结果
	vector<vector<MyMesh::VertexHandle>> res2 = autoDetectionbySameVertex(mesh);
	for (int i = 0; i < res2.size(); i++)
	{
		res.push_back(res2[i]);
	}



	/*vector<OpenMesh::VertexHandle> vHandle;
	for (MyMesh::VertexIter v_it = m_pre_mesh.vertices_begin(); v_it != m_pre_mesh.vertices_end(); ++v_it)
	vHandle.push_back(*v_it);
	cout << computeDistance(m_pre_mesh, vHandle[9], vHandle[11]);*/
	return res;
}
