#pragma once
#include <wx/wfstream.h>
#include "glm/vec2.hpp"
#include <vector>

class cGraph
{
public:
	enum class Norm{Euclidean,Taxicab,Maximum,Lp,Custom};

	void AddVertex(double x, double y);
	void AddVertex(const glm::dvec2& v);
	void RemoveVertex(size_t vindex);
	void MoveVertex(size_t vindex, const glm::dvec2& v);
	void Clear(const char* capt);
	
	void SetNorm(std::string newnorm);
	double GetDistance(size_t v0index, size_t v1index);
	void SetDistance(size_t v0index, size_t v1index, double dist);

	void WriteToFile(wxFileOutputStream& file);
	void ReadFromFile(wxFileInputStream& file);

	inline std::vector<glm::dvec2>::iterator begin() { return m_vertices.begin(); }
	inline std::vector<glm::dvec2>::const_iterator cbegin() { return m_vertices.cbegin(); }
	inline std::vector<glm::dvec2>::iterator end() { return m_vertices.end(); }
	inline std::vector<glm::dvec2>::const_iterator cend() { return m_vertices.cend(); }
	inline size_t size() { return m_vertices.size(); }
	inline glm::dvec2 operator[] (size_t index) const { return m_vertices[index]; }

private:
	std::vector<glm::dvec2> m_vertices = {};
	std::vector<double> m_weights = {}; //Used for customised distances The distance v0->v1 is stored as the v1*(v1-1)/2 + v0
	Norm m_norm = Norm::Euclidean;
	double m_Lp = 2; //Used for L-p distances

	bool m_editable = true; //Used to disable editing of a graph while running an algorithm
};

