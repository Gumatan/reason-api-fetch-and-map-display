[@bs.val]
external getItem: string => Js.Nullable.t(string) = "localStorage.getItem";

[@bs.val] external setItem: (string, string) => unit = "localStorage.setItem";