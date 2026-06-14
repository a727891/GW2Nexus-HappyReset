#pragma once

#include "mumble/Mumble.h"

namespace hr {
namespace MumbleIdentity {

float ParseUiScale(const Mumble::Data* data);
int ParseUisz(const Mumble::Data* data);

}  // namespace MumbleIdentity
}  // namespace hr
