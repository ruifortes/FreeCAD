/****************************************************************************
 *   Copyright (c) 2017 Zheng, Lei (realthunder) <realthunder.dev@gmail.com>*
 *                                                                          *
 *   This file is part of the FreeCAD CAx development system.               *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Library General Public            *
 *   License as published by the Free Software Foundation; either           *
 *   version 2 of the License, or (at your option) any later version.       *
 *                                                                          *
 *   This library  is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Library General Public License for more details.                   *
 *                                                                          *
 *   You should have received a copy of the GNU Library General Public      *
 *   License along with this library; see the file COPYING.LIB. If not,     *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,          *
 *   Suite 330, Boston, MA  02111-1307, USA                                 *
 *                                                                          *
 ****************************************************************************/

#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include <Gui/Application.h>
#include <Mod/Path/App/FeatureArea.h>
#include "ViewProviderArea.h"

using namespace PathGui;

PROPERTY_SOURCE(PathGui::ViewProviderArea, PartGui::ViewProviderPlaneParametric)

ViewProviderArea::ViewProviderArea()
{
    sPixmap = "Path-Area.svg";
}

ViewProviderArea::~ViewProviderArea()
{
}

std::vector<App::DocumentObject*> ViewProviderArea::claimChildren(void) const
{
    return std::vector<App::DocumentObject*>(
            static_cast<Path::FeatureArea*>(getObject())->Sources.getValues());
}

bool ViewProviderArea::canDragObjects() const
{
    return true;
}

bool ViewProviderArea::canDragObject(App::DocumentObject* obj) const
{
    return obj && obj->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId());
}

void ViewProviderArea::dragObject(App::DocumentObject* obj)
{
    Path::FeatureArea* area = static_cast<Path::FeatureArea*>(getObject());
    std::vector<App::DocumentObject*> sources = area->Sources.getValues();
    for (std::vector<App::DocumentObject*>::iterator it = sources.begin(); it != sources.end(); ++it) {
        if (*it == obj) {
            sources.erase(it);
            area->Sources.setValues(sources);
            break;
        }
    }
}

bool ViewProviderArea::canDropObjects() const
{
    return true;
}

bool ViewProviderArea::canDropObject(App::DocumentObject* obj) const
{
    return canDragObject(obj);
}

void ViewProviderArea::dropObject(App::DocumentObject* obj)
{
    Path::FeatureArea* area = static_cast<Path::FeatureArea*>(getObject());
    std::vector<App::DocumentObject*> sources = area->Sources.getValues();
    sources.push_back(obj);
    area->Sources.setValues(sources);
}

void ViewProviderArea::updateData(const App::Property* prop)
{
    PartGui::ViewProviderPart::updateData(prop);
    if (prop->getTypeId() == App::PropertyLinkList::getClassTypeId()) {
        std::vector<App::DocumentObject*> pShapes = static_cast<const App::PropertyLinkList*>(prop)->getValues();
        for (std::vector<App::DocumentObject*>::iterator it = pShapes.begin(); it != pShapes.end(); ++it) {
            if (*it)
                Gui::Application::Instance->hideViewProvider(*it);
        }
    }
}

bool ViewProviderArea::onDelete(const std::vector<std::string> &)
{
    // get the input shapes
    Path::FeatureArea* area = static_cast<Path::FeatureArea*>(getObject());
    std::vector<App::DocumentObject*> pShapes =area->Sources.getValues();
    for (std::vector<App::DocumentObject*>::iterator it = pShapes.begin(); it != pShapes.end(); ++it) {
        if (*it)
            Gui::Application::Instance->showViewProvider(*it);
    }
    return true;
}

// Python object -----------------------------------------------------------------------

namespace Gui {
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(PathGui::ViewProviderAreaPython, PathGui::ViewProviderArea)
/// @endcond

// explicit template instantiation
template class PathGuiExport ViewProviderPythonFeatureT<PathGui::ViewProviderArea>;
}
