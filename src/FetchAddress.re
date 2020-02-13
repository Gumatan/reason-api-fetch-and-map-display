[@bs.val] external fetch: string => Js.Promise.t('a) = "fetch";

type feature = {
  .
  "geometry": {
    .
    "coordinates": (float, float),
    "type": string,
  },
  "properties": {
    .
    "city": string,
    "citycode": string,
    "context": string,
    "housenumber": string,
    "id": string,
    "importance": float,
    "label": string,
    "name": string,
    "postcode": string,
    "score": float,
    "street": string,
    "type": string,
    "x": float,
    "y": float,
  },
  "type": string,
};

type state =
  | WaitingForUserSearch
  | LoadingAddress
  | ErrorFetchingAddress
  | NoResults
  | LoadedAddress(array(feature));

external featureToJson: array(feature) => Js.Json.t = "%identity";

type option('a) =
  | None
  | Some('a);

[@react.component]
let make = () => {
  let (state, setState) = React.useState(() => WaitingForUserSearch);
  let (query: string, setQuery) = React.useState(() => "");
  // React.useEffect0(() => {
  //   let features = LocalStorage.getItem("features")->Js.Nullable.toOption;

  //   switch (features) {
  //   | None => ()
  //   | Some(features) => setState(_previousState => LoadedAddress(features))
  //   };

  //   None;
  // });
  let transformSpacesToPlusInQuery = (string: string) => {
    let transformedQuery = ref("");
    for (i in 0 to Js.String.length(string) - 1) {
      transformedQuery :=
        transformedQuery^
        ++ (
          switch (string.[i]) {
          | ' ' => "+"
          | _ => String.make(1, string.[i])
          }
        );
    };
    transformedQuery^;
  };

  let fetchAddress = _event =>
    if (query !== "") {
      setState(_previousState => LoadingAddress);
      Js.Promise.(
        fetch(
          "https://api-adresse.data.gouv.fr/search/?q="
          ++ query->transformSpacesToPlusInQuery,
        )
        |> then_(response => response##json())
        |> then_(jsonResponse => {
             Js.log(jsonResponse);
             switch (Js.Array.length(jsonResponse##features)) {
             | 0 => setState(_previousState => NoResults)
             | _ =>
               //  LocalStorage.setItem(
               //    "features",
               //    jsonResponse##features->featureToJson,
               //  );
               setState(_previousState =>
                 LoadedAddress(jsonResponse##features)
               )
             };

             Js.Promise.resolve();
           })
        |> catch(_err => {
             Js.log(_err);
             setState(_previousState => ErrorFetchingAddress);
             Js.Promise.resolve();
           })
        |> ignore
      );
    };

  <>
    <div style={ReactDOMRe.Style.make(~height="120px", ())}>
      <form
        onSubmit={e => {
          e->ReactEvent.Form.preventDefault;
          fetchAddress();
        }}>
        <input
          value=query
          onChange={event => {setQuery(event->ReactEvent.Form.target##value)}}
        />
        <button type_="submit"> {React.string("Search")} </button>
      </form>
      {switch (state) {
       | WaitingForUserSearch => React.string("")
       | ErrorFetchingAddress => React.string("An error occurred!")
       | LoadingAddress => React.string("Loading...")
       | NoResults => React.string("No match found on gouvernment database")
       | LoadedAddress(featureCollection) =>
         featureCollection
         ->Belt.Array.map(feature => {
             <div key={feature##properties##id}>
               {feature##properties##label->React.string}
             </div>
           })
         ->React.array
       }}
    </div>
    <Map defaultCenter=[|50.879, 4.6997|]>
      <Marker anchor=[|50.879, 4.6997|] payload=1 />
    </Map>
  </>;
};