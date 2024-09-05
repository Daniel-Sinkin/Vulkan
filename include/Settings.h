#ifndef SETTINGS_H
#define SETTINGS_H

namespace Settings {
// For example macbooks have integrated graphics cards, so they would be filtered by this, which wouldn't make sense
constexpr bool ALLOW_DEVICE_WITHOUT_INTEGRATED_GPU = true;
} // namespace Settings
#endif // SETTINGS_H