---
name: EduNexus Command Center
colors:
  surface: '#f7f9fb'
  surface-dim: '#d8dadc'
  surface-bright: '#f7f9fb'
  surface-container-lowest: '#ffffff'
  surface-container-low: '#f2f4f6'
  surface-container: '#eceef0'
  surface-container-high: '#e6e8ea'
  surface-container-highest: '#e0e3e5'
  on-surface: '#191c1e'
  on-surface-variant: '#44474e'
  inverse-surface: '#2d3133'
  inverse-on-surface: '#eff1f3'
  outline: '#74777f'
  outline-variant: '#c4c6cf'
  surface-tint: '#465f88'
  primary: '#000a1e'
  on-primary: '#ffffff'
  primary-container: '#002147'
  on-primary-container: '#708ab5'
  inverse-primary: '#aec7f6'
  secondary: '#515f74'
  on-secondary: '#ffffff'
  secondary-container: '#d5e3fc'
  on-secondary-container: '#57657a'
  tertiary: '#000d06'
  on-tertiary: '#ffffff'
  tertiary-container: '#002718'
  on-tertiary-container: '#009c6b'
  error: '#ba1a1a'
  on-error: '#ffffff'
  error-container: '#ffdad6'
  on-error-container: '#93000a'
  primary-fixed: '#d6e3ff'
  primary-fixed-dim: '#aec7f6'
  on-primary-fixed: '#001b3d'
  on-primary-fixed-variant: '#2d476f'
  secondary-fixed: '#d5e3fc'
  secondary-fixed-dim: '#b9c7df'
  on-secondary-fixed: '#0d1c2e'
  on-secondary-fixed-variant: '#3a485b'
  tertiary-fixed: '#6ffbbe'
  tertiary-fixed-dim: '#4edea3'
  on-tertiary-fixed: '#002113'
  on-tertiary-fixed-variant: '#005236'
  background: '#f7f9fb'
  on-background: '#191c1e'
  surface-variant: '#e0e3e5'
typography:
  display-lg:
    fontFamily: Inter
    fontSize: 32px
    fontWeight: '700'
    lineHeight: 40px
    letterSpacing: -0.02em
  headline-md:
    fontFamily: Inter
    fontSize: 24px
    fontWeight: '600'
    lineHeight: 32px
    letterSpacing: -0.01em
  headline-sm:
    fontFamily: Inter
    fontSize: 20px
    fontWeight: '600'
    lineHeight: 28px
  body-lg:
    fontFamily: Inter
    fontSize: 16px
    fontWeight: '400'
    lineHeight: 24px
  body-md:
    fontFamily: Inter
    fontSize: 14px
    fontWeight: '400'
    lineHeight: 20px
  label-md:
    fontFamily: Inter
    fontSize: 12px
    fontWeight: '600'
    lineHeight: 16px
    letterSpacing: 0.05em
  mono-sm:
    fontFamily: JetBrains Mono
    fontSize: 12px
    fontWeight: '500'
    lineHeight: 16px
rounded:
  sm: 0.25rem
  DEFAULT: 0.5rem
  md: 0.75rem
  lg: 1rem
  xl: 1.5rem
  full: 9999px
spacing:
  base: 8px
  xs: 4px
  sm: 12px
  md: 16px
  lg: 24px
  xl: 32px
  xxl: 48px
  gutter: 24px
  margin: 32px
---

## Brand & Style
The brand personality is **Institutional Modern**: a synthesis of academic prestige and contemporary efficiency. It is designed for university administrators who require a high-trust environment to manage complex data and student records. The UI must evoke a sense of stability, precision, and clarity.

The chosen style is **Corporate / Modern** with a lean toward **Minimalism**. It prioritizes a highly structured information architecture over decorative elements. Visual weight is used strategically to guide the eye through dense data sets, ensuring that the "Command Center" feels like a powerful, reliable instrument rather than just a software interface.

## Colors
The palette is rooted in **Oxford Blue (#002147)**, providing an immediate sense of institutional authority and "ivy league" heritage. This is the primary color for navigation, headers, and core actions.

**Slate Grey (#475569)** serves as the secondary color, used for supporting text and icons to maintain high legibility without the harshness of pure black. **Emerald Green (#10B981)** is the functional accent, reserved strictly for optimization indicators, successful statuses, and recommended pathways.

The background uses a subtle **Off-White (#F8FAFC)** to reduce eye strain, allowing **Pure White (#FFFFFF)** cards and containers to pop forward in the visual hierarchy.

## Typography
This design system utilizes **Inter** for all primary interfaces. Inter’s tall x-height and neutral character make it ideal for data-heavy administrative views. 

- **Headlines:** Use Semi-Bold (600) or Bold (700) weight with slight negative letter-spacing to maintain a compact, authoritative look.
- **Body:** Standardized at 14px for density, scaling to 16px for narrative or help text.
- **Labels:** Small, uppercase labels with increased tracking are used for table headers and metadata categories.
- **Monospaced:** For specific technical outputs like Student IDs or Algorithm Status codes, **JetBrains Mono** is used at a small scale to signal "system data."

## Layout & Spacing
The layout follows a **12-column Fixed Grid** on desktop (max-width: 1440px) to ensure data density remains predictable and readable. A strict **8px spacing system** governs all margins and paddings.

- **Desktop:** 32px outer margins with 24px gutters. Sidebars are fixed at 280px.
- **Tablet:** 24px outer margins. Sidebar collapses into a rail or drawer.
- **Mobile:** 16px margins. Layout reflows to a single column with horizontal scrolling enabled for data tables.

Generous whitespace is prioritized between functional groups (sections) while maintaining tight proximity within related data points (form fields).

## Elevation & Depth
Depth is conveyed through **Tonal Layering** and **Low-Contrast Outlines**. 

Shadows are used sparingly to prevent the UI from feeling "heavy." When used, they are **Ambient Shadows**: high blur, very low opacity (4-8%), tinted slightly with the primary Navy color to maintain harmony.

- **Level 0 (Background):** #F8FAFC.
- **Level 1 (Cards/Surface):** #FFFFFF with a 1px border (#E2E8F0).
- **Level 2 (Dropdowns/Modals):** #FFFFFF with a soft ambient shadow (0px 4px 20px rgba(0, 33, 71, 0.08)).
- **Level 3 (Popovers):** Sharp 1px borders with a secondary accent shadow to indicate immediate focus.

## Shapes
The shape language is disciplined and consistent. A **Rounded (0.5rem / 8px)** base radius is applied to cards, input fields, and buttons. This softening prevents the "Institutional" look from feeling overly cold or dated.

Larger containers like modals or main content areas use `rounded-lg` (16px), while small elements like tags or "Algorithm Status" pips use `rounded-sm` (4px) to maintain a precise, technical feel.

## Components
- **Buttons:** Primary buttons use Oxford Blue with white text. Ghost buttons use Slate Grey outlines. All buttons have a subtle 1px inset top-border to suggest a physical "press" surface.
- **Algorithm Status Bar:** A specialized component featuring a thin track with an Emerald Green glow. It uses monospaced text to show "Processing..." or "Optimized" status.
- **Data Tables:** Pure white backgrounds with 1px Oxford Blue (at 10% opacity) horizontal dividers. Hover states on rows should use a very faint Slate Grey tint (#F1F5F9).
- **Input Fields:** 8px rounded corners, 1px Slate Grey border. On focus, the border transitions to Oxford Blue with a 2px outer "halo" of the same color at 10% opacity.
- **Status Chips:** Small, pill-shaped indicators. "Optimized" uses Emerald Green background (10% opacity) with Emerald Green text. "Pending" uses Slate Grey.
- **Cards:** White surface, 1px border (#E2E8F0), 8px corner radius. No shadow unless the card is interactive or draggable.