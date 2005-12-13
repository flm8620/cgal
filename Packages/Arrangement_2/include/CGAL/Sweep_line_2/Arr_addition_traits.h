// Copyright (c) 1997  Tel-Aviv University (Israel).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Source$
// $Revision$ $Date$
// $Name$
//
// Author(s)     : Baruch Zukerman <baruchzu@post.tau.ac.il>

#ifndef ARR_ADDITION_TRAITS_H
#define ARR_ADDITION_TRAITS_H

#include <CGAL/Object.h> 
#include <list>
#include <iterator>

CGAL_BEGIN_NAMESPACE

template <class Traits_, class Arrangement_>
class Arr_addition_traits 
{
public:

  typedef Traits_                                  Traits_2;
  typedef Arrangement_                             Arrangement_2;
  typedef typename Traits_2::Curve_2               Curve_2;

  typedef typename Arrangement_2::Halfedge_handle  Halfedge_handle;
  typedef typename Arrangement_2::Vertex_handle    Vertex_handle;
  typedef typename Traits_2::X_monotone_curve_2    Base_x_monotone_curve_2;
  typedef typename Traits_2::Point_2               Base_point_2;
  typedef typename Traits_2::Intersect_2           Base_Intersect_2;
  typedef typename Traits_2::Split_2               Base_Split_2;
  typedef typename Traits_2::Make_x_monotone_2     Base_Make_x_monotone_2;
  typedef typename Traits_2::Construct_min_vertex_2
                                                   Base_Construct_min_vertex_2;
  typedef typename Traits_2::Construct_max_vertex_2
                                                   Base_Construct_max_vertex_2;
  typedef typename Traits_2::Compare_xy_2          Base_Compare_xy_2;
  typedef typename Traits_2::Compare_y_at_x_2      Base_Compare_y_at_x_2;
  typedef typename Traits_2::Compare_y_at_x_right_2 
                                                   Base_Compare_y_at_x_right_2;
  typedef typename Traits_2::Equal_2               Base_Equal_2;
  typedef typename Traits_2::Compare_x_2           Base_Compare_x_2;
  typedef typename Traits_2::Is_vertical_2         Base_Is_vertical_2;

protected:

  Traits_2*    m_base_traits;

public:

  //Constructor
  Arr_addition_traits (Traits_2& tr):
    m_base_traits (&tr)
  {}

  /*! \class
   * Nested extension of the x-monotone curve type.
   */
  class Ex_x_monotone_curve_2 
  {
  public:

    typedef  Base_x_monotone_curve_2 Base;
    typedef  Base_point_2            Point_2;

    Ex_x_monotone_curve_2():m_base_cv(),
                            m_he_handle(NULL)
    {}

    Ex_x_monotone_curve_2(const Base& cv):m_base_cv(cv),
                                          m_he_handle(NULL)
    {}

    Ex_x_monotone_curve_2(const Base& cv, Halfedge_handle he):m_base_cv(cv),
                                                              m_he_handle(he)
    {}

    const Base& base() const
    {
      return (m_base_cv);
    }

    Base& base()
    {
      return (m_base_cv);
    }

    operator const Base& () const
    {
      return (m_base_cv);
    }

    Ex_x_monotone_curve_2& operator=(const Base& cv)
    {
      m_base_cv = cv;
      m_he_handle = Halfedge_handle();
      return (*this);
    }

    Halfedge_handle get_halfedge_handle() const
    {
      return m_he_handle;
    }

    void set_halfedge_handle(Halfedge_handle he)
    {
      m_he_handle = he;
    }

  protected:
    Base                m_base_cv;
    Halfedge_handle     m_he_handle;

  };

  /*! \class
   * Nested extension of the point type.
   */
  class Ex_point_2 
  {
  public:
    typedef  Base_point_2            Base;

    Ex_point_2(): m_base_pt(),
                  m_v(NULL)
    {}

    Ex_point_2(const Base& pt): m_base_pt(pt),
                                m_v(NULL)
    {}

    Ex_point_2(const Base& pt, Vertex_handle v): m_base_pt(pt),
                                                 m_v(v)
    {}

    const Base& base() const
    {
      return (m_base_pt);
    }
    
    operator const Base& () const
    {
      return (m_base_pt);
    }

    Vertex_handle get_vertex_handle() const
    {
      return m_v;
    }

    void set_vertex_handle(Vertex_handle v)
    {
      m_v = v;
    }
    
  protected:
    Base             m_base_pt;
    Vertex_handle    m_v;

  };

  
  typedef Ex_x_monotone_curve_2                     X_monotone_curve_2;
  typedef Ex_point_2                                Point_2;

  /*! \class
   * The Intersect_2 functor.
   */
  class Intersect_2
  {
  private:

    Base_Intersect_2      m_base_intersect;

  public:
   
    /*! Constructor. */
    Intersect_2 (const Base_Intersect_2& base) :
        m_base_intersect (base)
    {}

    template<class OutputIterator>
    OutputIterator operator() (const X_monotone_curve_2& cv1,
                               const X_monotone_curve_2& cv2,
                               OutputIterator oi)
    {
      if(cv1.get_halfedge_handle() != Halfedge_handle(NULL) &&
         cv2.get_halfedge_handle() != Halfedge_handle(NULL) )
        return (oi); // the curves are disjoint-interior because they
                     // are already at the Arrangement

      OutputIterator           oi_end = m_base_intersect(cv1.base(),
                                                         cv2.base(), oi);
      const Base_x_monotone_curve_2                *overlap_cv;
      const std::pair<Base_point_2, unsigned int>  *intersect_p;

      // convert objects that are associated with Base_x_monotone_curve_2 to
      // X_monotone_curve_2 
      for(; oi != oi_end; ++oi)
      {
        overlap_cv = object_cast<Base_x_monotone_curve_2> (&(*oi));
        if (overlap_cv != NULL)
        {
          // Add halfedge handles to the resulting curve.
          Halfedge_handle  he;

          if (cv1.get_halfedge_handle() != Halfedge_handle())
            he = cv1.get_halfedge_handle();
          else if (cv2.get_halfedge_handle() != Halfedge_handle())
            he = cv2.get_halfedge_handle();

          *oi = make_object (X_monotone_curve_2 (*overlap_cv, he));
        }
        else
        {
          intersect_p = 
            object_cast<std::pair<Base_point_2, unsigned int> > (&(*oi));

          CGAL_assertion (intersect_p != NULL);

          *oi = make_object (std::make_pair (Point_2(intersect_p->first),
                                             intersect_p->second));
        }
      }

      // Return a past-the-end iterator.
      return oi_end;
    }
  };

  Intersect_2 intersect_2_object () 
  {
    return (Intersect_2 (m_base_traits->intersect_2_object())); 
  }

  /*! \class
   * The Split_2 functor.
   */
  class Split_2
  {
  private:
    Base_Split_2    m_base_split;

  public:

    /*! Constructor. */
    Split_2 (const Base_Split_2& base) :
        m_base_split (base)
    {}

    void operator() (const X_monotone_curve_2& cv, const Point_2 & p,
                     X_monotone_curve_2& c1, X_monotone_curve_2& c2)
    {
      m_base_split(cv.base(),
                   p.base(),
                   c1.base(),
                   c2.base());
      c1.set_halfedge_handle(cv.get_halfedge_handle());
      c2.set_halfedge_handle(cv.get_halfedge_handle());
    }
  };

  Split_2 split_2_object () 
  {
    return (Split_2 (m_base_traits->split_2_object()));
  }

  /*! \class
   * The Make_x_monotone_2 functor.
   */
  class Make_x_monotone_2
  {
  private:
    Base_Make_x_monotone_2    m_base_make_x;
    std::list<Object>         m_objects;

  public:

    /*! Constructor. */
    Make_x_monotone_2 (const Base_Make_x_monotone_2& base) :
        m_base_make_x (base)
    {}

    template<class OutputIterator>
    OutputIterator operator() (const typename Traits_2::Curve_2& cv,
                               OutputIterator oi) 
    {
      const Base_x_monotone_curve_2   *xcv;
      const Base_point_2              *pt;

      m_base_make_x(cv, std::back_inserter(m_objects));
      for(std::list<Object>::iterator iter = m_objects.begin();
          iter != m_objects.end();
          ++iter)
      {
        if ((xcv = object_cast<Base_x_monotone_curve_2> (&(*iter))) != NULL)
        {
          *oi = make_object (X_monotone_curve_2 (*xcv));
        }
        else
        {
          pt = object_cast<Base_point_2> (&(*iter));
          CGAL_assertion (pt != NULL);

          *oi = make_object (Point_2 (*pt));
        }
        ++oi;
      }
      m_objects.clear();
      return oi;
    }

  };

  Make_x_monotone_2 make_x_monotone_2_object () 
  {
    return (Make_x_monotone_2 (m_base_traits->make_x_monotone_2_object()));
  }

  /*! \class
   * The Construct_min_vertex_2 functor.
   */
  class Construct_min_vertex_2
  {
  private:
    Base_Construct_min_vertex_2 m_base_min_v;

  public:

    Construct_min_vertex_2(const Base_Construct_min_vertex_2& base):
        m_base_min_v(base)
    {}

    Point_2 operator() (const X_monotone_curve_2 & cv) 
    {

      if(cv.get_halfedge_handle() == Halfedge_handle())
        return (Point_2(m_base_min_v(cv.base()), Vertex_handle()));

      Vertex_handle vh = cv.get_halfedge_handle()->target();
      return (Point_2(m_base_min_v(cv.base()), vh));
    }
  };

  Construct_min_vertex_2 construct_min_vertex_2_object () const
  {
    return (Construct_min_vertex_2
	    (m_base_traits->construct_min_vertex_2_object()));
  }

  /*! \class
   * The Construct_max_vertex_2 functor.
   */
  class Construct_max_vertex_2
  {
  private:
    Base_Construct_max_vertex_2 m_base_max_v;

  public:

    Construct_max_vertex_2(const Base_Construct_max_vertex_2& base):
        m_base_max_v(base)
    {}

    Point_2 operator() (const X_monotone_curve_2 & cv) 
    {
      if(cv.get_halfedge_handle() == Halfedge_handle())
        return (Point_2(m_base_max_v(cv.base()), Vertex_handle()));

      Vertex_handle vh = cv.get_halfedge_handle()->source();
      return (Point_2(m_base_max_v(cv.base()), vh));
    }
  };

  Construct_max_vertex_2 construct_max_vertex_2_object () const
  {
    return (Construct_max_vertex_2
	    (m_base_traits->construct_max_vertex_2_object()));
  }

  /*! \class
   * The Comapre_xy_2 functor.
   */
  class Compare_xy_2
  {
  private:
    Base_Compare_xy_2 m_base_cmp_xy;

  public:

    Compare_xy_2(const Base_Compare_xy_2& base):
        m_base_cmp_xy(base)
    {}

    Comparison_result operator() (const Point_2& p1, const Point_2& p2) const
    {
      if(p1.get_vertex_handle() == p2.get_vertex_handle() &&
         p1.get_vertex_handle() != Vertex_handle())
        return EQUAL;

      return (m_base_cmp_xy(p1.base(), p2.base()));
    }
  };

  Compare_xy_2 compare_xy_2_object () 
  {
    return (Compare_xy_2 (m_base_traits->compare_xy_2_object()));
  }

  /*! \class
   * The Comapre_y_at_x_2 functor.
   */
  class Compare_y_at_x_2
  {
  private:
    Base_Compare_y_at_x_2 m_base_cmp_y_at_x;

  public:
    Compare_y_at_x_2(const Base_Compare_y_at_x_2& base):
        m_base_cmp_y_at_x(base)
    {}

    Comparison_result operator() (const Point_2& p,
                                  const X_monotone_curve_2& cv) const
    {
      return (m_base_cmp_y_at_x(p.base(), cv.base()));
    }
  };

  Compare_y_at_x_2 compare_y_at_x_2_object () 
  {
    return (Compare_y_at_x_2 (m_base_traits->compare_y_at_x_2_object()));
  }

  /*! \class
   * The Comapre_y_at_x_right_2 functor.
   */
  class Compare_y_at_x_right_2
  {
  private:
    Base_Compare_y_at_x_right_2 m_base_cmp_y_at_x_right;

  public:
    Compare_y_at_x_right_2(const Base_Compare_y_at_x_right_2& base):
        m_base_cmp_y_at_x_right(base)
    {}

    Comparison_result operator() (const X_monotone_curve_2& cv1,
                                  const X_monotone_curve_2& cv2,
                                  const Point_2& p) const
    {
      return (m_base_cmp_y_at_x_right(cv1.base(),
                                      cv2.base(),
                                      p.base()));
    }
  };

  Compare_y_at_x_right_2 compare_y_at_x_right_2_object () 
  {
    return (Compare_y_at_x_right_2
	    (m_base_traits->compare_y_at_x_right_2_object()));
  }

  /*! \class
   * The Equal_2 functor.
   */
  class Equal_2
  {
  private:
    Base_Equal_2 m_base_eq;

  public:
    
    Equal_2(const Base_Equal_2& base):
        m_base_eq(base)
    {}

    /*! Check if two curves are the same. */
    bool operator() (const X_monotone_curve_2& cv1,
		     const X_monotone_curve_2& cv2) const
    {
      return (m_base_eq(cv1.base(), cv2.base()));
    }

    /*! Check if the two points are the same. */
    bool operator() (const Point_2& p1, const Point_2& p2) const
    {
      return (m_base_eq(p1.base(), p2.base()));
    }
  };

  Equal_2 equal_2_object () 
  {
    return (Equal_2 (m_base_traits->equal_2_object()));
  }

  /*! \class
   * The Comapre_x_2 functor.
   */
  class Compare_x_2
  {
  private:
    Base_Compare_x_2 m_base_cmp_x;

  public:
    Compare_x_2(const Base_Compare_x_2& base):
        m_base_cmp_x(base)
    {}

     bool operator() (const Point_2& p1, const Point_2& p2) const
    {
      return (m_base_cmp_x(p1.base(), p2.base()));
    }
  };

  Compare_x_2 compare_x_2_object () 
  {
    return (Compare_x_2 (m_base_traits->compare_x_2_object()));
  }

  /*! \class
   * The Is_vertical_2 functor.
   */
  class Is_vertical_2
  {
  private:
    Base_Is_vertical_2 m_base_is_vert;

  public:
    Is_vertical_2(const Base_Is_vertical_2& base):
        m_base_is_vert(base)
    {}

     bool operator() (const X_monotone_curve_2& cv) const
    {
      return (m_base_is_vert(cv.base()));
    }
  };

  Is_vertical_2 is_vertical_2_object() 
  {
    return (Is_vertical_2(m_base_traits->is_vertical_2_object()));
  }
};


CGAL_END_NAMESPACE

#endif
