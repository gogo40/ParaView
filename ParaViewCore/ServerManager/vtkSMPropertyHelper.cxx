/*=========================================================================

  Program:   ParaView
  Module:    vtkSMPropertyHelper.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*
* Copyright (c) 2007, Sandia Corporation
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Sandia Corporation nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Sandia Corporation ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Sandia Corporation BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "vtkSMPropertyHelper.h"

#include "vtkObjectFactory.h"
#include "vtkSMDoubleVectorProperty.h"
#include "vtkSMIdTypeVectorProperty.h"
#include "vtkSMInputProperty.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMProxy.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkStringList.h"
#include "vtkSMEnumerationDomain.h"

#include <vtksys/ios/sstream>

#include <assert.h>
#include <algorithm>

#define vtkSMPropertyHelperWarningMacro(blah)\
  if (this->Quiet == false) \
  {\
    vtkGenericWarningMacro(blah)\
  }

//----------------------------------------------------------------------------
template<typename T>
inline T vtkSMPropertyHelper::GetProperty(unsigned int index) const
{
  (void) index;

  return T();
}

//----------------------------------------------------------------------------
template<>
inline int vtkSMPropertyHelper::GetProperty(unsigned int index) const
{
  switch(this->Type)
    {
    case INT:
      return this->IntVectorProperty->GetElement(index);
    case DOUBLE:
      return this->DoubleVectorProperty->GetElement(index);
    case IDTYPE:
      return this->IdTypeVectorProperty->GetElement(index);
    default:
      return 0;
    }
}

//----------------------------------------------------------------------------
template<>
inline double vtkSMPropertyHelper::GetProperty(unsigned int index) const
{
  switch(this->Type)
    {
    case INT:
      return this->IntVectorProperty->GetElement(index);
    case DOUBLE:
      return this->DoubleVectorProperty->GetElement(index);
    case IDTYPE:
      return this->IdTypeVectorProperty->GetElement(index);
    default:
      return 0;
    }
}

//----------------------------------------------------------------------------
template<>
inline vtkIdType vtkSMPropertyHelper::GetProperty(unsigned int index) const
{
  switch(this->Type)
    {
    case INT:
      return this->IntVectorProperty->GetElement(index);
    case IDTYPE:
      return this->IdTypeVectorProperty->GetElement(index);
    default:
      return 0;
    }
}

//----------------------------------------------------------------------------
template<>
inline const char* vtkSMPropertyHelper::GetProperty(unsigned int index) const
  {
  if(this->Type == STRING)
    {
    return this->StringVectorProperty->GetElement(index);
    }
  else if(this->Type == INT)
    {
    // enumeration domain
    vtkSMEnumerationDomain* domain = vtkSMEnumerationDomain::SafeDownCast(
          this->Property->FindDomain("vtkSMEnumerationDomain"));
    if(domain != NULL)
      {
      const char* entry = domain->GetEntryTextForValue(this->IntVectorProperty->GetElement(index));
      if(entry)
        {
        return entry;
        }
      }
    }

  return 0;
}

//----------------------------------------------------------------------------
template<>
inline vtkSMProxy* vtkSMPropertyHelper::GetProperty(unsigned int index) const
{
  switch(this->Type)
    {
    case PROXY:
    case INPUT:
      return this->ProxyProperty->GetProxy(index);
    default:
      return 0;
    }
}

//----------------------------------------------------------------------------
template<typename T>
inline std::vector<T> vtkSMPropertyHelper::GetPropertyArray() const
{
  std::vector<T> array;

  for(unsigned int i = 0; i < this->GetNumberOfElements(); i++)
    {
    array.push_back(this->GetProperty<T>(i));
    }

  return array;
}

//----------------------------------------------------------------------------
template<typename T>
unsigned int vtkSMPropertyHelper::GetPropertyArray(T *values, unsigned int count)
{
  count = std::min(count, this->GetNumberOfElements());

  for(unsigned int i = 0; i < count; i++)
    {
    values[i] = this->GetProperty<T>(i);
    }

  return count;
}

//----------------------------------------------------------------------------
template<typename T>
inline void vtkSMPropertyHelper::SetProperty(unsigned int index, T value)
{
  (void) index;
  (void) value;
}

//----------------------------------------------------------------------------
template<>
inline void vtkSMPropertyHelper::SetProperty(unsigned int index, int value)
{
  switch(this->Type)
    {
    case INT:
      this->IntVectorProperty->SetElement(index, value);
      break;
    case DOUBLE:
      this->DoubleVectorProperty->SetElement(index, value);
      break;
    case IDTYPE:
      this->IdTypeVectorProperty->SetElement(index, value);
      break;
    default:
      break;
    }
}

//----------------------------------------------------------------------------
template<>
inline void vtkSMPropertyHelper::SetProperty(unsigned int index, double value)
{
  switch(this->Type)
    {
    case INT:
      this->IntVectorProperty->SetElement(index, value);
      break;
    case DOUBLE:
      this->DoubleVectorProperty->SetElement(index, value);
      break;
    default:
      break;
    }
}

//----------------------------------------------------------------------------
template<>
inline void vtkSMPropertyHelper::SetProperty(unsigned int index, vtkIdType value)
{
  switch(this->Type)
    {
    case INT:
      this->IntVectorProperty->SetElement(index, value);
      break;
    case IDTYPE:
      this->IdTypeVectorProperty->SetElement(index, value);
      break;
    default:
      break;
    }
}

template<>
inline void vtkSMPropertyHelper::SetProperty(unsigned int index, const char *value)
{
  if(this->Type == STRING)
    {
    this->StringVectorProperty->SetElement(index, value);
    }
  else if(this->Type == INT)
    {
    // enumeration domain
    vtkSMEnumerationDomain* domain = vtkSMEnumerationDomain::SafeDownCast(
      this->Property->FindDomain("vtkSMEnumerationDomain"));
    if (domain != NULL && domain->HasEntryText(value))
      {
      int valid; // We already know that the entry exist...
      this->IntVectorProperty->SetElement(index, domain->GetEntryValue(value, valid));
      }
    }
  else
    {
    vtkSMPropertyHelperWarningMacro("Call not supported for the current property type.");
    }
}

//----------------------------------------------------------------------------
template<typename T>
inline void vtkSMPropertyHelper::SetPropertyArray(const T *values, unsigned int count)
{
  (void) values;
  (void) count;
}

//----------------------------------------------------------------------------
template<>
inline void vtkSMPropertyHelper::SetPropertyArray(const int *values, unsigned int count)
{
  if(this->Type == INT)
    {
    this->IntVectorProperty->SetElements(values, count);
    }
  else
    {
    vtkSMPropertyHelperWarningMacro("Call not supported for the current property type.");
    }
}

//----------------------------------------------------------------------------
template<>
inline void vtkSMPropertyHelper::SetPropertyArray(const double *values, unsigned int count)
{
  if(this->Type == DOUBLE)
    {
    this->DoubleVectorProperty->SetElements(values, count);
    }
  else
    {
    vtkSMPropertyHelperWarningMacro("Call not supported for the current property type.");
    }
}

//----------------------------------------------------------------------------
template<>
inline void vtkSMPropertyHelper::SetPropertyArray(const vtkIdType *values, unsigned int count)
{
  if(this->Type == IDTYPE)
    {
    this->IdTypeVectorProperty->SetElements(values, count);
    }
  else
    {
    vtkSMPropertyHelperWarningMacro("Call not supported for the current property type.");
    }
}

//----------------------------------------------------------------------------
vtkSMPropertyHelper::vtkSMPropertyHelper(vtkSMProxy* proxy, const char* pname,
  bool quiet)
{
  this->Proxy = proxy;
  this->Quiet = quiet;

  vtkSMProperty *property = proxy->GetProperty(pname);

  if(!property)
    {
    vtkSMPropertyHelperWarningMacro("Failed to locate property: " << pname);
    }

  this->Initialize(property);
}

//----------------------------------------------------------------------------
vtkSMPropertyHelper::vtkSMPropertyHelper(vtkSMProperty *property, bool quiet)
{
  this->Proxy = 0;
  this->Quiet = quiet;

  this->Initialize(property);
}

//----------------------------------------------------------------------------
vtkSMPropertyHelper::~vtkSMPropertyHelper()
{
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::Initialize(vtkSMProperty *property)
{
  this->Property = property;
  this->Type = vtkSMPropertyHelper::NONE;

  if(property != NULL)
    {
    if (property->IsA("vtkSMIntVectorProperty"))
      {
      this->Type = vtkSMPropertyHelper::INT;
      }
    else if (property->IsA("vtkSMDoubleVectorProperty"))
      {
      this->Type = vtkSMPropertyHelper::DOUBLE;
      }
    else if (property->IsA("vtkSMIdTypeVectorProperty"))
      {
      this->Type = vtkSMPropertyHelper::IDTYPE;
      }
    else if (property->IsA("vtkSMStringVectorProperty"))
      {
      this->Type = vtkSMPropertyHelper::STRING;
      }
    else if (property->IsA("vtkSMInputProperty"))
      {
      this->Type = vtkSMPropertyHelper::INPUT;
      }
    else if (property->IsA("vtkSMProxyProperty"))
      {
      this->Type = vtkSMPropertyHelper::PROXY;
      }
    else
      {
      vtkSMPropertyHelperWarningMacro("Unhandled property type : " << property->GetClassName());
      }
    }
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::UpdateValueFromServer()
{
  if(this->Proxy)
    {
    this->Proxy->UpdatePropertyInformation(this->Property);
    }
  else
    {
    vtkGenericWarningMacro("No proxy set.");
    }
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::SetNumberOfElements(unsigned int elems)
{
  switch (this->Type)
    {
    case INT:
    case DOUBLE:
    case IDTYPE:
    case STRING:
      this->VectorProperty->SetNumberOfElements(elems);
      break;
    case PROXY:
    case INPUT:
      this->ProxyProperty->SetNumberOfProxies(elems);
      break;
    default:
      vtkSMPropertyHelperWarningMacro("Call not supported for the current property type.");
    }
}

//----------------------------------------------------------------------------
unsigned int vtkSMPropertyHelper::GetNumberOfElements() const
{
  switch (this->Type)
    {
    case INT:
    case DOUBLE:
    case IDTYPE:
    case STRING:
      return this->VectorProperty->GetNumberOfElements();
    case PROXY:
    case INPUT:
      return this->ProxyProperty->GetNumberOfProxies();
    default:
      vtkSMPropertyHelperWarningMacro("Call not supported for the current property type.");
      return 0;
    }
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::Set(unsigned int index, int value)
{
  this->SetProperty<int>(index, value);
}

//----------------------------------------------------------------------------
int vtkSMPropertyHelper::GetAsInt(unsigned int index /*=0*/)
{
  return this->GetProperty<int>(index);
}

//----------------------------------------------------------------------------
unsigned int vtkSMPropertyHelper::Get(int *values, unsigned int count /*=1*/)
{
  return this->GetPropertyArray<int>(values, count);
}

//----------------------------------------------------------------------------
std::vector<int> vtkSMPropertyHelper::GetIntArray()
{
  return this->GetPropertyArray<int>();
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::Set(const int* values, unsigned int count)
{
  this->SetPropertyArray<int>(values, count);
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::Set(unsigned int index, double value)
{
  this->SetProperty<double>(index, value);
}

//----------------------------------------------------------------------------
double vtkSMPropertyHelper::GetAsDouble(unsigned int index /*=0*/)
{
  return this->GetProperty<double>(index);
}

//----------------------------------------------------------------------------
unsigned int vtkSMPropertyHelper::Get(double *values, unsigned int count /*=1*/)
{
  return this->GetPropertyArray<double>(values, count);
}

//----------------------------------------------------------------------------
std::vector<double> vtkSMPropertyHelper::GetDoubleArray()
{
  return this->GetPropertyArray<double>();
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::Set(const double* values, unsigned int count)
{
  this->SetPropertyArray<double>(values, count);
}

#if VTK_SIZEOF_ID_TYPE != VTK_SIZEOF_INT
//----------------------------------------------------------------------------
void vtkSMPropertyHelper::Set(unsigned int index, vtkIdType value)
{
  this->SetProperty<vtkIdType>(index, value);
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::Set(const vtkIdType* values, unsigned int count)
{
  this->SetPropertyArray<vtkIdType>(values, count);
}

//----------------------------------------------------------------------------
unsigned int vtkSMPropertyHelper::Get(vtkIdType* values, unsigned int count /*=1*/)
{
  return this->GetPropertyArray<vtkIdType>(values, count);
}
#endif

//----------------------------------------------------------------------------
vtkIdType vtkSMPropertyHelper::GetAsIdType(unsigned int index /*=0*/)
{
  return this->GetProperty<vtkIdType>(index);
}

//----------------------------------------------------------------------------
std::vector<vtkIdType> vtkSMPropertyHelper::GetIdTypeArray()
{
  return this->GetPropertyArray<vtkIdType>();
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::Set(unsigned int index, const char* value)
{
  this->SetProperty<const char*>(index, value);
}

//----------------------------------------------------------------------------
const char* vtkSMPropertyHelper::GetAsString(unsigned int index /*=0*/)
{
  return this->GetProperty<const char*>(index);
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::Set(unsigned int index, vtkSMProxy* value, 
  unsigned int outputport/*=0*/)
{
  if (this->Type == PROXY)
    {
    this->ProxyProperty->SetProxy(index, value);
    }
  else if (this->Type == INPUT)
    {
    this->InputProperty->SetInputConnection(index, value, outputport);
    }
  else
    {
    vtkSMPropertyHelperWarningMacro("Call not supported for the current property type.");
    }
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::Set(vtkSMProxy** value, unsigned int count, 
  unsigned int *outputports/*=NULL*/)
{
  if (this->Type == PROXY)
    {
    this->ProxyProperty->SetProxies(count, value);
    }
  else if (this->Type == INPUT)
    {
    this->InputProperty->SetProxies(count, value, outputports);
    }
  else
    {
    vtkSMPropertyHelperWarningMacro("Call not supported for the current property type.");
    }
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::Add(vtkSMProxy* value, unsigned int outputport/*=0*/)
{
  if (this->Type == PROXY)
    {
    this->ProxyProperty->AddProxy(value);
    }
  else if (this->Type == INPUT)
    {
    this->InputProperty->AddInputConnection(value, outputport);
    }
  else
    {
    vtkSMPropertyHelperWarningMacro("Call not supported for the current property type.");
    }
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::Remove(vtkSMProxy* value)
{
  if(this->Type == PROXY ||
     this->Type == INPUT)
    {
    this->ProxyProperty->RemoveProxy(value);
    }
  else
    {
    vtkSMPropertyHelperWarningMacro("Call not supported for the current property type.");
    }
}

//----------------------------------------------------------------------------
vtkSMProxy* vtkSMPropertyHelper::GetAsProxy(unsigned int index/*=0*/)
{
  return this->GetProperty<vtkSMProxy*>(index);
}

//----------------------------------------------------------------------------
unsigned int vtkSMPropertyHelper::GetOutputPort(unsigned int index/*=0*/)
{
  if (this->Type == INPUT)
    {
    return this->InputProperty->GetOutputPortForConnection(index);
    }

  vtkSMPropertyHelperWarningMacro("Call not supported for the current property type.");
  return 0;
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::SetStatus(const char* key, int value)
{
  if (this->Type != vtkSMPropertyHelper::STRING)
    {
    vtkSMPropertyHelperWarningMacro("Status properties can only be vtkSMStringVectorProperty.");
    return;
    }

  vtkSMStringVectorProperty* svp = vtkSMStringVectorProperty::SafeDownCast(
    this->Property);
  if (svp->GetNumberOfElementsPerCommand() != 2)
    {
    vtkSMPropertyHelperWarningMacro("NumberOfElementsPerCommand != 2");
    return;
    }

  if (!svp->GetRepeatCommand())
    {
    vtkSMPropertyHelperWarningMacro("Property is non-repeatable.");
    return;
    }


  vtksys_ios::ostringstream str;
  str << value;

  for (unsigned int cc=0; (cc+1) < svp->GetNumberOfElements(); cc+=2)
    {
    if (strcmp(svp->GetElement(cc), key) == 0)
      {
      svp->SetElement(cc+1, str.str().c_str());
      return;
      }
    }

  vtkStringList* list = vtkStringList::New();
  svp->GetElements(list);
  list->AddString(key);
  list->AddString(str.str().c_str());
  svp->SetElements(list);
  list->Delete();
}

//----------------------------------------------------------------------------
int vtkSMPropertyHelper::GetStatus(const char* key, int default_value/*=0*/)
{
  if (this->Type != vtkSMPropertyHelper::STRING)
    {
    vtkSMPropertyHelperWarningMacro("Status properties can only be vtkSMStringVectorProperty.");
    return default_value;
    }

  vtkSMStringVectorProperty* svp = vtkSMStringVectorProperty::SafeDownCast(
    this->Property);
  while (svp)
    {
    if (svp->GetNumberOfElementsPerCommand() != 2)
      {
      vtkSMPropertyHelperWarningMacro("NumberOfElementsPerCommand != 2");
      return default_value;
      }

    if (!svp->GetRepeatCommand())
      {
      vtkSMPropertyHelperWarningMacro("Property is non-repeatable.");
      return default_value;
      }

    for (unsigned int cc=0; (cc+1) < svp->GetNumberOfElements(); cc+=2)
      {
      if (strcmp(svp->GetElement(cc), key) == 0)
        {
        return atoi(svp->GetElement(cc+1));
        }
      }

    // Now check if the information_property has the value.
    svp = svp->GetInformationOnly() == 0? 
      vtkSMStringVectorProperty::SafeDownCast(svp->GetInformationProperty()) : 0;
    }

  return default_value;
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::SetStatus(const char* key, double *values,
  int num_values)
{
  if (this->Type != vtkSMPropertyHelper::STRING)
    {
    vtkSMPropertyHelperWarningMacro("Status properties can only be vtkSMStringVectorProperty.");
    return;
    }

  vtkSMStringVectorProperty* svp = vtkSMStringVectorProperty::SafeDownCast(
    this->Property);
  if (svp->GetNumberOfElementsPerCommand() != num_values+1)
    {
    vtkSMPropertyHelperWarningMacro("NumberOfElementsPerCommand != " << num_values + 1);
    return;
    }

  if (!svp->GetRepeatCommand())
    {
    vtkSMPropertyHelperWarningMacro("Property is non-repeatable.");
    return;
    }


  vtkStringList* list = vtkStringList::New();
  svp->GetElements(list);

  bool append = true;
  for (unsigned int cc=0; (cc+num_values+1) <= svp->GetNumberOfElements();
    cc+=(num_values+1))
    {
    if (strcmp(svp->GetElement(cc), key) == 0)
      {
      for (int kk=0; kk < num_values; kk++)
        {
        vtksys_ios::ostringstream str;
        str << values[kk];
        list->SetString(cc+kk+1, str.str().c_str());
        }
      append = false;
      }
    }

  if (append)
    {
    list->AddString(key);
    for (int kk=0; kk < num_values; kk++)
      {
      vtksys_ios::ostringstream str;
      str << values[kk];
      list->AddString(str.str().c_str());
      }
    }
  svp->SetElements(list);
  list->Delete();
}

//----------------------------------------------------------------------------
bool vtkSMPropertyHelper::GetStatus(const char* key, double *values, int num_values)
{
  if (this->Type != vtkSMPropertyHelper::STRING)
    {
    vtkSMPropertyHelperWarningMacro("Status properties can only be vtkSMStringVectorProperty.");
    return false;
    }

  vtkSMStringVectorProperty* svp = vtkSMStringVectorProperty::SafeDownCast(
    this->Property);

  while (svp)
    {
    if (svp->GetNumberOfElementsPerCommand() != num_values+1)
      {
      vtkSMPropertyHelperWarningMacro("NumberOfElementsPerCommand != " << num_values + 1);
      return false;
      }

    if (!svp->GetRepeatCommand())
      {
      vtkSMPropertyHelperWarningMacro("Property is non-repeatable.");
      return false;
      }

    for (unsigned int cc=0; (cc+num_values+1) <= svp->GetNumberOfElements();
      cc+=(num_values+1))
      {
      if (strcmp(svp->GetElement(cc), key) == 0)
        {
        for (int kk=0; kk < num_values; kk++)
          {
          values[kk] = atof(svp->GetElement(cc+kk+1));
          }
        return true;
        }
      }

    // Now check if the information_property has the value.
    svp = svp->GetInformationOnly() == 0? 
      vtkSMStringVectorProperty::SafeDownCast(svp->GetInformationProperty()) : 0;
    }

  return false;
}

//----------------------------------------------------------------------------
void vtkSMPropertyHelper::SetStatus(const char* key, const char* value)
{
  if (this->Type != vtkSMPropertyHelper::STRING)
    {
    vtkSMPropertyHelperWarningMacro("Status properties can only be vtkSMStringVectorProperty.");
    return;
    }

  vtkSMStringVectorProperty* svp = vtkSMStringVectorProperty::SafeDownCast(
    this->Property);
  if (svp->GetNumberOfElementsPerCommand() != 2)
    {
    vtkSMPropertyHelperWarningMacro("NumberOfElementsPerCommand != 2");
    return;
    }

  if (!svp->GetRepeatCommand())
    {
    vtkSMPropertyHelperWarningMacro("Property is non-repeatable.");
    return;
    }

  for (unsigned int cc=0; (cc+1) < svp->GetNumberOfElements(); cc+=2)
    {
    if (strcmp(svp->GetElement(cc), key) == 0)
      {
      svp->SetElement(cc+1, value);
      return;
      }
    }

  vtkStringList* list = vtkStringList::New();
  svp->GetElements(list);
  list->AddString(key);
  list->AddString(value);
  svp->SetElements(list);
  list->Delete();
}

//----------------------------------------------------------------------------
const char* vtkSMPropertyHelper::GetStatus(const char* key, const char* default_value)
{
  if (this->Type != vtkSMPropertyHelper::STRING)
    {
    vtkSMPropertyHelperWarningMacro("Status properties can only be vtkSMStringVectorProperty.");
    return default_value;
    }

  vtkSMStringVectorProperty* svp = vtkSMStringVectorProperty::SafeDownCast(
    this->Property);
  while (svp)
    {
    if (svp->GetNumberOfElementsPerCommand() != 2)
      {
      vtkSMPropertyHelperWarningMacro("NumberOfElementsPerCommand != 2");
      return default_value;
      }

    if (!svp->GetRepeatCommand())
      {
      vtkSMPropertyHelperWarningMacro("Property is non-repeatable.");
      return default_value;
      }

    for (unsigned int cc=0; (cc+1) < svp->GetNumberOfElements(); cc+=2)
      {
      if (strcmp(svp->GetElement(cc), key) == 0)
        {
        return svp->GetElement(cc+1);
        }
      }

    // Now check if the information_property has the value.
    svp = svp->GetInformationOnly() == 0? 
      vtkSMStringVectorProperty::SafeDownCast(svp->GetInformationProperty()) : 0;
    }

  return default_value;
}

