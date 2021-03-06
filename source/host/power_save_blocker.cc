//
// Aspia Project
// Copyright (C) 2019 Dmitry Chapyshev <dmitry@aspia.ru>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#include "host/power_save_blocker.h"

#include "base/logging.h"

#include <utility>

namespace host {

namespace {

HANDLE createPowerRequest(POWER_REQUEST_TYPE type, const std::wstring_view& description)
{
    REASON_CONTEXT context = { 0 };

    context.Version = POWER_REQUEST_CONTEXT_VERSION;
    context.Flags = POWER_REQUEST_CONTEXT_SIMPLE_STRING;
    context.Reason.SimpleReasonString = const_cast<wchar_t*>(description.data());

    base::win::ScopedHandle handle(PowerCreateRequest(&context));
    if (!handle.isValid())
    {
        PLOG(LS_WARNING) << "PowerCreateRequest failed";
        return INVALID_HANDLE_VALUE;
    }

    if (!PowerSetRequest(handle, type))
    {
        PLOG(LS_WARNING) << "PowerSetRequest failed";
        return INVALID_HANDLE_VALUE;
    }

    return handle.release();
}

// Takes ownership of the |handle|.
void deletePowerRequest(POWER_REQUEST_TYPE type, HANDLE handle)
{
    base::win::ScopedHandle request_handle(handle);
    if (!request_handle.isValid())
        return;

    BOOL success = PowerClearRequest(request_handle, type);
    DCHECK(success);
}

} // namespace

PowerSaveBlocker::PowerSaveBlocker()
{
    static const wchar_t kDescription[] = L"Aspia session is active";

    // The display will always be on as long as the class instance exists.
    handle_.reset(createPowerRequest(PowerRequestDisplayRequired, kDescription));
}

PowerSaveBlocker::~PowerSaveBlocker()
{
    deletePowerRequest(PowerRequestDisplayRequired, handle_.release());
}

} // namespace host
