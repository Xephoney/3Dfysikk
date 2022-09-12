#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <QDebug>
#include <iterator>

namespace gsml
{
    template <class ID, class T>
    class QuadTree;

    typedef std::pair<double, double> Point2D; // Erstatter Vector2D klasse

    // Template klasse, laget for ID og T
    template <class ID, class T>
    class QuadTree
    {
    private:
        // Punkter som definerer hjørnene til objektet
        // i samme rekkefølge som pekerne til subtrær
        Point2D m_a;
        Point2D m_b;
        Point2D m_c;
        Point2D m_d;
        // Pekere til subtrær er nødvendig i en dynamisk
        // quadtre implementering. Vi kunne ha lagt det
        // i array også
        QuadTree* m_sv{nullptr};
        QuadTree* m_so{nullptr};
        QuadTree* m_no{nullptr};
        QuadTree* m_nv{nullptr};
        //                      B L      B R      T R      T L
        QuadTree* m_subTrees[4]{nullptr, nullptr, nullptr, nullptr};
        // Hvert quad lagrer sine egne objekter
        // (T kan være objekter, pekere eller referanser)
        // Legger opp til mulig søk på id
        std::list<std::pair<ID,T>> m_objects;

        //sjekker om det eksisterer subtreer og hvis ikke returnerer den sann.
        bool isLeaf() const
        {
            bool result = !(m_sv || m_so || m_no || m_nv);
            //qDebug() << "Leafcheck = " << result;
            return result;
        }

        // Hjelpevariabel for å lagre alle T-ene
        // for senere iterering
        std::vector<T> m_all_objects; // skulle vært static

        // Hjelpefunksjon - preorder traversering av subtre
        void traverse_all(std::vector<T>& all_objects)
        {
            for(auto it = m_objects.begin();it!=m_objects.end();it++)
            {
                if((*it).second != nullptr)
                    all_objects.push_back(it->second);
                else
                    m_objects.erase(it);
            }
            if(m_no)
                m_no->traverse_all(all_objects);
            if(m_nv)
                m_nv->traverse_all(all_objects);
            if(m_sv)
                m_sv->traverse_all(all_objects);
            if(m_so)
                m_so->traverse_all(all_objects);
        }

        inline static bool s_bTreeModified;
    protected:
        QuadTree* m_parent {nullptr};

    public:
        QuadTree() : m_sv{nullptr}, m_so{nullptr}, m_no{nullptr} ,m_nv{nullptr}
        {
            s_bTreeModified = {true};
        }
        QuadTree(const Point2D &v1, const Point2D &v2, const Point2D &v3, const Point2D & v4)
            : m_a{v1}, m_b{v2}, m_c{v3}, m_d{v4}
        {
            s_bTreeModified = {true};
        }
        ~QuadTree()
        {
            for(auto it = 0; it < m_all_objects.size();it++)
            {
                delete m_all_objects[it];
            }
            m_all_objects.clear();
        }
        void init(const Point2D &v1, const Point2D &v2, const Point2D &v3, const Point2D & v4)
        {
            m_a=v1;
            m_b=v2;
            m_c=v3;
            m_d=v4;
            m_sv={nullptr};
            m_so={nullptr};
            m_no={nullptr};
            m_nv={nullptr};
            s_bTreeModified = {true};
        }
        void init(const int& size)
        {
            const float minX = -((float)size/2.f);
            const float minY = minX;

            const float maxX = ((float)size/2.f);
            const float maxY = maxX;

            m_a = Point2D(minX, minY);
            m_b = Point2D(maxX, minY);
            m_c = Point2D(maxX, maxY);
            m_d = Point2D(minX, maxY);

            m_sv={nullptr};
            m_nv={nullptr};
            m_so={nullptr};
            m_no={nullptr};
            s_bTreeModified = {true};
        }
        void subDivide(int n)
        {
            if(n>0)
            {
                Point2D v1 = Point2D{(m_a.first+m_b.first)/2.f,(m_a.second+m_b.second)/2.f};
                Point2D v2 = Point2D{(m_b.first+m_c.first)/2.f,(m_b.second+m_c.second)/2.f};
                Point2D v3 = Point2D{(m_c.first+m_d.first)/2.f,(m_c.second+m_d.second)/2.f};
                Point2D v4 = Point2D{(m_d.first+m_a.first)/2.f,(m_d.second+m_a.second)/2.f};
                Point2D m = Center();

                m_sv = new QuadTree(m_a,v1,m,v4);
                m_sv->m_parent = this;
                m_sv->subDivide(n-1);

                m_so = new QuadTree(v1,m_b,v2,m);
                m_so->m_parent = this;
                m_so->subDivide(n-1);

                m_no = new QuadTree(m,v2,m_c,v3);
                m_no->m_parent = this;
                m_no->subDivide(n-1);

                m_nv = new QuadTree(v4,m,v3,m_d);
                m_nv->m_parent = this;
                m_nv->subDivide(n-1);
            }
        }
        void print() const
        {
            Point2D cent = Point2D{(m_a.first+m_c.first)/2,(m_a.second+m_c.second)/2};

            std::cout << "Center = (" << cent.first <<", "<<cent.second <<")\n" ;
            for(auto it=m_objects.begin();it!=m_objects.end();it++)
                std::cout << "|\t("<<it->first<<", " << it->second->getNavn() << "\n";
        };
        void print_all() const
        {
            print();
            if(m_no)
                m_no->print_all();
            if(m_nv)
                m_nv->print_all();
            if(m_so)
                m_so->print_all();
            if(m_sv)
                m_sv->print_all();
        }
        QuadTree* insert(const Point2D& point, const ID& id, const T& object)
        {
            if(isLeaf())
            {
                m_objects.push_back(std::pair<ID,T>(id,object));
                s_bTreeModified=true;
                return this;
            }

            Point2D m = Center();

            // Vertical slice! Nord eller sør?
            if(point.second < m.second)
            {
                //Sør
                if(point.first < m.first)
                    return m_sv->insert(point, id, object);
                else
                    return m_so->insert(point, id, object);
            }
            else
            {
                //Nord
                if(point.first < m.first)
                    return m_nv->insert(point, id, object);
                else
                    return m_no->insert(point, id, object);
            }
        }
        QuadTree* find(const Point2D& p, bool parent = false)
        {
            if(isLeaf() && !parent)
                return this;

            if(isLeaf() && parent && m_parent)
                return m_parent;

            Point2D m = Center();
            // Vertical slice! Nord eller sør?
            if(p.second < m.second)
            {
                //Sør
                if(p.first < m.first)
                    return m_sv->find(p,parent);
                else
                    return m_so->find(p,parent);
            }
            else
            {
                //Nord
                if(p.first < m.first)
                    return m_nv->find(p,parent);
                else
                    return m_no->find(p,parent);
            }
        }
        const QuadTree* find(const Point2D& p) const
        {
            if(isLeaf())
                return this;


            Point2D m = Center();

            //Nord eller sør?
            if(p.second < m.second)
            {
                //Sør
                if(p.first < m.first)
                    return m_sv->find(p); // Vest
                else
                    return m_so->find(p); // Øst
            }
            else
            {
                //Nord

                //Øst eller Vest?
                if(p.first < m.first)
                    return m_nv->find(p); // Vest
                else
                    return m_no->find(p); // Øst
            }
        }
        T& operator[] (ID id)
        {
            return find(id);
        }
        T find(const ID& id) const
        {
            if(isLeaf() && m_objects.size() > 0)
            {
                //Loop igjennom mObjects og finne den som matcher ID og returnere .second
                for(auto it = m_objects.begin(); it!=m_objects.end(); it++)
                {
                    if((*it).first == id)
                    {
                        return (*it).second;
                    }
                }
                return NULL;
            }
            else
            {
                QuadTree* subTrees[4] {m_sv, m_so, m_no, m_nv};
                for(int i = 0 ; i < 4; i++)
                {
                    if(subTrees[i] != nullptr)
                    {
                        T obj = subTrees[i]->find(id);
                        if(obj != NULL)
                            return obj;
                    }
                }
                return NULL;
            }
        }
        Point2D Center() const
        {
            return {(m_a.first+m_c.first)/2.f, (m_a.second+m_c.second)/2.f};
        }

        void Delete(const ID& id)
        {
            if(isLeaf() && m_objects.size() > 0)
            {
                for(auto it = m_objects.begin(); it!=m_objects.end(); it++)
                {
                    if((*it).first == id)
                    {
                        delete (*it).second;
                        m_objects.erase(it);
                        s_bTreeModified = {true};
                        return;
                    }
                }
            }
            else
            {
                QuadTree* subTrees[4] {m_sv, m_so, m_no, m_nv};
                for(int i = 0 ; i < 4; i++)
                {
                    if(subTrees[i] != nullptr)
                    {
                        subTrees[i]->Delete(id);
                    }
                }
            }
        }
        int ObjectCount()
        {
            return m_all_objects.size();
        }
        std::vector<std::pair<ID,T>> fetchObjects()
        {
            std::vector<std::pair<ID,T>> tempVec;
            if(isLeaf())
            {
                tempVec.insert(tempVec.begin(), m_objects.begin(), m_objects.end());
                return tempVec;
            }
            else
            {

                std::vector<std::pair<ID,T>> childObj;
                childObj = m_sv->fetchObjects();
                tempVec.insert(tempVec.end(),childObj.begin(),childObj.end());

                childObj = m_so->fetchObjects();
                tempVec.insert(tempVec.end(),childObj.begin(),childObj.end());

                childObj = m_no->fetchObjects();
                tempVec.insert(tempVec.end(),childObj.begin(),childObj.end());

                childObj = m_nv->fetchObjects();
                tempVec.insert(tempVec.end(),childObj.begin(),childObj.end());

                return tempVec;
            }
        }
        typename std::vector<T>::iterator begin()
        {
            //Sjekker om det har skjedd noe siden sist begin() ble kalt
            // og oppdaterer m_all_objects hvis det stemmer.
            if(s_bTreeModified)
            {
                m_all_objects.clear();
                traverse_all(m_all_objects);
                s_bTreeModified = {false};
            }

            return m_all_objects.begin();
        }
        typename std::vector<T>::iterator end()
        {
            return m_all_objects.end();
        }
    };
}


//                qDebug() << "Inserted " << id.c_str() << "at this subtree:";
//                qDebug() << "Center = (" << Center().first << ", " << Center().second << ")";
//                qDebug() << "Point = (" <<point.first<<", "<<point.second<<")";
//                qDebug() << "A("<<m_a.first<<", "<<m_a.second<<")";
//                qDebug() << "B("<<m_b.first<<", "<<m_b.second<<")";
//                qDebug() << "C("<<m_c.first<<", "<<m_c.second<<")";
//                qDebug() << "D("<<m_d.first<<", "<<m_d.second<<")";
