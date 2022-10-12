using System;
using System.IO;
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Reflection;
using System.ComponentModel;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.CodeDom.Compiler;
using Microsoft.CSharp;

namespace Decoder
{
	class Program
	{
		static void Main(string[] args)
		{
			// decoder [r / w] [Assembly-CSharp.dll Path] [Save File Path] (JSON)
			// I don't care about untrusted args[] input cause that won't happen. :^)

			Assembly assemblyCSharp = Assembly.LoadFrom(args[1]);

			FileStream file;
			BinaryFormatter formatter = new BinaryFormatter();
			if (args[0] == "r") { // args[0] isn't the binary name, I am upset.
				file = File.Open(args[2], FileMode.Open);
				Object save = formatter.Deserialize(file);
				file.Close();

				Console.WriteLine("{");
				FieldInfo[] fields = save.GetType().GetFields();
				foreach (FieldInfo f in fields) {
					Console.Write("  \"" + f.Name + "\": ");
					if (f.FieldType.IsArray) {
						Console.Write("[\"" + f.FieldType.FullName + "\", ");
						Console.Write("[");
						int i = 1;
						var values = (dynamic)(f.GetValue(save));
						foreach (var v in values) {
							Console.Write(v.ToString().ToLower());
							if (i != values.Length) Console.Write(", ");
							i++;
						}
						Console.Write("]]");
					} else Console.Write("[\"" + f.FieldType.FullName + "\", " + f.GetValue(save).ToString().ToLower() + "]");

					if (f == fields.Last()) Console.WriteLine("");
					else Console.WriteLine(",");
				}
				Console.WriteLine("}");
			} else if (args[0] == "w") {
				file = File.Open(args[2], FileMode.Open);
				Object save = formatter.Deserialize(file);
				file.Close();
				Type type = save.GetType();

				JsonDocument json = JsonDocument.Parse(File.ReadAllText(args[3]));
				JsonElement root = json.RootElement;
				foreach (JsonProperty property in root.EnumerateObject()) {
					FieldInfo field = type.GetField(property.Name, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
					Type castType = Type.GetType(property.Value[0].GetString());
					field.SetValue(save, property.Value[1].Deserialize(castType));
					Console.WriteLine(property.Name + " = " + field.GetValue(save).ToString());
				}

				FileStream output = File.Open(args[2], FileMode.Open);
				try {
					formatter.Serialize(output, save);
				} catch (SerializationException e) {
					Console.WriteLine("Failed to serialize. Reason: " + e.Message);
					throw;
				}
				output.Close();
			}
		}
	}
}
