[@bs.val]
external getItem: string => Js.Nullable.t(string) = "localStorage.getItem";

[@bs.val]
external setItem: (string, Js.Json.t) => unit = "localStorage.setItem";