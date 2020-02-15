[@bs.module "pigeon-marker"] [@react.component]
external make: (~anchor: array(float), ~payload: int) => React.element =
  "default";