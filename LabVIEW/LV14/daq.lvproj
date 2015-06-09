<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="14008000">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="jsonToLibera.vi" Type="VI" URL="../daq/jsonToLibera.vi"/>
		<Item Name="libera.vi" Type="VI" URL="../daq/libera.vi"/>
		<Item Name="libera_get.vi" Type="VI" URL="../daq/libera_get.vi"/>
		<Item Name="liberaToCmd.vi" Type="VI" URL="../daq/liberaToCmd.vi"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Check if File or Folder Exists.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/Check if File or Folder Exists.vi"/>
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
				<Item Name="LabVIEWHTTPClient.lvlib" Type="Library" URL="/&lt;vilib&gt;/httpClient/LabVIEWHTTPClient.lvlib"/>
				<Item Name="NI_FileType.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/lvfile.llb/NI_FileType.lvlib"/>
				<Item Name="NI_PackedLibraryUtility.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/LVLibp/NI_PackedLibraryUtility.lvlib"/>
				<Item Name="Path To Command Line String.vi" Type="VI" URL="/&lt;vilib&gt;/AdvancedString/Path To Command Line String.vi"/>
				<Item Name="PathToUNIXPathString.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/CFURL.llb/PathToUNIXPathString.vi"/>
				<Item Name="Search and Replace Pattern.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Search and Replace Pattern.vi"/>
			</Item>
			<Item Name="ChaosDevState.ctl" Type="VI" URL="../../../../chaos-common-crest/LabVIEW/LV14/chaosui-crest/VIs/ChaosDevState.ctl"/>
			<Item Name="chaosui-crest-cu-setstate.vi" Type="VI" URL="../../../../chaos-common-crest/LabVIEW/LV14/chaosui-crest/VIs/chaosui-crest-cu-setstate.vi"/>
			<Item Name="chaosui-crest.lvlib" Type="Library" URL="../../../../chaos-common-crest/LabVIEW/LV14/chaosui-crest/chaosui-crest.lvlib"/>
			<Item Name="chaosui-default-decode.vi" Type="VI" URL="../../../../chaos-common-crest/LabVIEW/LV14/chaosui-crest/VIs/chaosui-default-decode.vi"/>
			<Item Name="libcommon-crest.so" Type="Document" URL="libcommon-crest.so">
				<Property Name="NI.PreserveRelativePath" Type="Bool">true</Property>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
