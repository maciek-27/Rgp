#include <iostream>
#include <cmath>
#include "horizontalgroup.h++"
#include <algorithm>
using std::min;
using namespace Scr;
using namespace Scr::Tk;

void HorizontalGroup::ArrangeContents()throw()
{
	Uint maxwidth = GetWidth();

	float totalmax = 0;
	Uint totalmin = 0;
	Uint coefsum = 0;

	Uint freestep = 0;

	bool stretchmax = false;

	Uint visible_elems =  elements.size();
	for(WidgetList::iterator i = elements.begin();
		i != elements.end(); i++) {

		if((*i)->IsHidden()) {
			visible_elems--;
			continue;
		}
		
		if((*i)->GetMaxWidth() == UintMax) // means, stretch to max
			stretchmax = true;
		else
			totalmax += (*i)->GetMaxWidth();
		// sum of coefficients will give some hint in dividing free
		// space			
		coefsum += elementsLayout[*i].stretchFactor;
		
		totalmin += (*i)->GetMinWidth();
	}	
	bool usecoef = false;
	if(totalmax > maxwidth || stretchmax) {
		totalmax = maxwidth; // use whole available space
		usecoef = true;
	}

	Uint addpoint = 0; // point at from which the first widget will be drawn
	switch(alignPolicy) {
	case Distribute:
	case Begin:
		addpoint = 0;
		break;
	case Center:
		/* NOTE: (GetHeight() - totalmax)/2 can result in overflow first
		   during substraction and the division would divide the overflown
		   value. More desired behaviour is achieved by the below operation.
		*/
		addpoint = GetWidth()/2 - totalmax/2;
		break;
	case End:
		addpoint = GetWidth() - totalmax;
		break;
	}

	if(usecoef) { // base on coefficients space division

		totalmax = maxwidth;

		if(totalmin <= totalmax)
			totalmax -= totalmin; // totalmax will be now a height that
		// has to be distributed among elements
		else
			totalmax = 0;
		
		for(WidgetList::iterator i = elements.begin();
			i != elements.end(); i++) {
			
			if((*i)->IsHidden())
				continue;

			Widget &w = **i;
			Uint coef = elementsLayout[&w].stretchFactor;			

			Uint distspace = roundf(((float)coef/(float)coefsum) * totalmax);

			if(w.GetMinWidth() >= distspace)
				distspace = 0;
			else
				distspace -= w.GetMinWidth();
			
			if(distspace + w.GetMinWidth() > w.GetMaxWidth())
				distspace = w.GetMaxWidth() - w.GetMinWidth();			
			
			// height = distributed space + minimal height of this widget
			w.SetSize(min(GetHeight(),w.GetMaxHeight()), distspace + w.GetMinWidth());
			
			totalmax -= distspace;
			coefsum -= coef;
		}
		if(totalmax && visible_elems) { // distribute anything left to the
			// first visible element that can aquire any more size
			Widget *w;

			for(WidgetList::iterator i = elements.begin();
				i != elements.end(); i++) {
				w=*i;
				if((*i)->IsHidden() ||
				   (*i)->GetWidth() + totalmax > (*i)->GetMaxHeight())
					continue;
				break;
			}
			w->SetSize(w->GetHeight(), w->GetWidth() + totalmax);
		}
	}
	else { // base on align model
		if(alignPolicy == Distribute)
			freestep = (maxwidth - totalmax)/elements.size();
		
		Uint tmp = addpoint;
		for(WidgetList::iterator i = elements.begin();
			i != elements.end(); i++) {
			if((*i)->IsHidden())
				continue;			
			Widget &w = **i;
			
			tmp += w.GetMaxWidth();
			w.SetSize(min(GetHeight(),w.GetMaxHeight()), w.GetMaxWidth());
		}
	}

	// finally position all the elements,stacking the heights
	for(WidgetList::iterator i = elements.begin();
		i != elements.end(); i++) {
		if((*i)->IsHidden())
			continue;
		Widget &w = **i;
		
		w.SetPosition(0, addpoint);
		addpoint += w.GetWidth() + freestep/*from the distributed model*/;
	}	

}

HorizontalGroup::HorizontalGroup(Uint _height,	
							 Uint _width,
							 const DisplayStyle &  _style)throw()
    :BoxGroup(_height, _width, _style)
{;}

HorizontalGroup::HorizontalGroup(const WidgetGroup & base)throw()
    :BoxGroup(base)
{;}

HorizontalGroup::~HorizontalGroup()throw(){;}
