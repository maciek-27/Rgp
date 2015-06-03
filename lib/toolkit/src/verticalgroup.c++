#include <iostream>
#include <cmath>
#include "verticalgroup.h++"

using namespace Scr;
using namespace Scr::Tk;

void VerticalGroup::ArrangeContents()throw()
{
	Uint maxheight = GetHeight();
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

		if((*i)->GetMaxHeight() == UintMax) // means, stretch to max
			stretchmax = true;
		else
			totalmax += (*i)->GetMaxHeight();
		// sum of coefficients will give some hint in dividing free
		// space			
		coefsum += elementsLayout[*i].stretchFactor;
		
		totalmin += (*i)->GetMinHeight();
	}	
	bool usecoef = false;
	if(totalmax > maxheight || stretchmax) {
		totalmax = maxheight; // use whole available space
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
		addpoint = GetHeight()/2 - totalmax/2;
		break;
	case End:
		addpoint = GetHeight() - totalmax;
		break;
	}

	if(usecoef) { // base on coefficients space division

		totalmax = maxheight;

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

			Uint distspace =roundf(((float)coef/(float)coefsum) * totalmax);

			if(w.GetMinHeight() >= distspace)
				distspace = 0;
			else
				distspace -= w.GetMinHeight();
			
			if(distspace + w.GetMinHeight() > w.GetMaxHeight())
				distspace = w.GetMaxHeight() - w.GetMinHeight();			
			
			// height = distributed space + minimal height of this widget
			w.SetSize(distspace + w.GetMinHeight(), maxwidth);
			
			totalmax -= distspace;
			coefsum -= coef;
		}
		if(totalmax && visible_elems) { // distribute anything left to the
			// first visible element that can aquire any more size
			Widget *w;
			for(WidgetList::iterator i = elements.begin();
				i != elements.end(); i++) {
				w = *i;				
				if(w->IsHidden() ||
				   w->GetHeight() + totalmax > w->GetMaxHeight())
					continue;
				break;
			}
			w->SetSize(w->GetHeight() + totalmax, w->GetWidth());
		}
	}
	else { // base on align model
		if(alignPolicy == Distribute)
			freestep = (maxheight - totalmax)/elements.size();
		
		Uint tmp = addpoint;
		for(WidgetList::iterator i = elements.begin();
			i != elements.end(); i++) {
			if((*i)->IsHidden())
				continue;		
			Widget &w = **i;
			
			tmp += w.GetMaxHeight();
			w.SetSize(w.GetMaxHeight(), maxwidth);
		}
	}

	// finally position all the elements,stacking the heights
	for(WidgetList::iterator i = elements.begin();
		i != elements.end(); i++) {
			if((*i)->IsHidden())
				continue;		
		Widget &w = **i;
		
		w.SetPosition(addpoint, 0);
		addpoint += w.GetHeight() + freestep/*from the distributed model*/;
	}	

}

VerticalGroup::VerticalGroup(Uint _height,	
							 Uint _width,
							 const DisplayStyle &  _style)throw()
    :BoxGroup(_height, _width,_style)
{
	;
}

VerticalGroup::VerticalGroup(const WidgetGroup & base)throw()
    :BoxGroup(base)
{;}

VerticalGroup::~VerticalGroup()throw() {;}
