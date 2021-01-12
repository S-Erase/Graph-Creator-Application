#include "cPCH.h"
#include "cGraph.h"

void cGraph::AddVertex(double x, double y)
{
	if(m_editable){
		if (m_vertices.size() < ((size_t)1 << 32) /*2^32*/) {
			m_weights.resize(m_weights.size() + m_vertices.size());
			m_vertices.emplace_back(x, y);
		}
		else {
			wxMessageBox("Maximum number of vertices has been reached");
		}
	}
}

void cGraph::AddVertex(const glm::dvec2& v)
{
	if (m_editable) {
		if (m_vertices.size() < ((size_t)1 << 32) /*2^32*/) {
			m_weights.resize(m_weights.size() + m_vertices.size());
			m_vertices.push_back(v);
		}
		else {
			wxMessageBox("Maximum number of vertices has been reached");
		}
	}
}

void cGraph::RemoveVertex(size_t vindex)
{
	if (m_editable) {
		for (int v1 = size()-1; v1 > vindex; v1--) {
			m_weights.erase(m_weights.begin() + v1 * (v1 - 1) / 2 + vindex);
		}
		for (int v0 = vindex-1; v0 >= 0; v0--) {
			m_weights.erase(m_weights.begin() + vindex * (vindex - 1) / 2 + v0);
		}

		m_vertices.erase(cbegin() + vindex);
	}

}

void cGraph::MoveVertex(size_t vindex, const glm::dvec2& v)
{
	if (m_editable) {
		m_vertices[vindex] = v;
	}
}

void cGraph::Clear(const char* capt) {
	if (m_editable) {
		int confirm = wxMessageBox("Are you sure you want to erase the current graph?",capt,wxYES_NO|wxCENTRE|wxNO_DEFAULT);
		if (confirm == wxYES) {
			m_vertices.clear();
			m_weights.clear();
		}
	}
}

void cGraph::SetNorm(std::string newnorm)
{
	if (m_editable) {
		if (newnorm == "Custom") {
			m_norm = Norm::Custom;
		}
		else if (newnorm == "Euclidean") {
			m_norm = Norm::Euclidean;
		}
		else if (newnorm == "Taxicab") {
			m_norm = Norm::Taxicab;
		}
		else if (newnorm == "Maximum") {
			m_norm = Norm::Maximum;
		}
		else if (newnorm.substr(0, 2) == "L-") {
			m_norm = Norm::Lp;
			m_Lp = stod(newnorm.substr(2, std::string::npos));
		}
	}
}

double cGraph::GetDistance(size_t v0index, size_t v1index)
{
	double dx = abs(m_vertices[v1index].x - m_vertices[v0index].x);
	double dy = abs(m_vertices[v1index].y - m_vertices[v0index].y);
	switch (m_norm) {
	case Norm::Euclidean: return sqrt(dx * dx + dy * dy);
	case Norm::Taxicab: return dx + dy;
	case Norm::Maximum: return std::max(dx,dy);
	case Norm::Lp: return pow(pow(dx,m_Lp) + pow(dy, m_Lp),1/m_Lp);
	case Norm::Custom:
		if (v0index > v1index) {
			return (m_weights[v0index * (v0index - 1) / 2 + v1index]);
		}
		else if (v0index < v1index) {
			return (m_weights[v1index * (v1index - 1) / 2 + v0index]);
		}
		else {
			return 0;
		}
		break;
	}
}

void cGraph::SetDistance(size_t v0index, size_t v1index, double dist)
{
	if (m_editable) {
		SE_ASSERT(m_norm == Norm::Custom, "Cannot change distance while the Norm is not set to Custom!");
		if (v0index > v1index) {
			m_weights[v0index * (v0index - 1) / 2 + v1index] = dist;
		}
		else if (v0index < v1index) {
			m_weights[v1index * (v1index - 1) / 2 + v0index] = dist;
		}
		else {
			SE_ASSERT(false, "Cannot set the distance from one vertex to itself!");
		}
	}
}

void cGraph::WriteToFile(wxFileOutputStream& file)
{
	file.SeekO(0);
	size_t m_size = m_vertices.size();
	file.Write(&m_size, sizeof(size_t));
	file.Write(m_vertices.data(), m_size * sizeof(glm::dvec2));
	file.Write(m_weights.data(), m_size * (m_size - 1) / 2 * sizeof(double));
	file.Write(&m_norm, sizeof(Norm));
	file.Write(&m_Lp, sizeof(double));
}

void cGraph::ReadFromFile(wxFileInputStream& file)
{
	if (m_editable) {
		file.SeekI(0);
		size_t m_size;
		file.Read(&m_size, sizeof(size_t));
		m_vertices.resize(m_size);
		file.Read(m_vertices.data(), m_size * sizeof(glm::dvec2));
		m_weights.resize(m_size * (m_size - 1) / 2);
		file.Read(m_weights.data(), m_size * (m_size - 1) / 2 * sizeof(double));
		file.Read(&m_norm, sizeof(Norm));
		file.Read(&m_Lp, sizeof(double));
	}
}
