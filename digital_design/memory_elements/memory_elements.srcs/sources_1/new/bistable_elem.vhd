----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 03:09:43 PM
-- Design Name: 
-- Module Name: bistable_elem - Structural
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity bistable_elem is
    Port ( nQ : out STD_LOGIC;
           Q : out STD_LOGIC);
end bistable_elem;

architecture Structural of bistable_elem is

component inv is
    Port ( I : in STD_LOGIC;
           Q : out STD_LOGIC);
end component;

signal inv_0i, inv_1i: std_logic;

begin
    INV0 : inv port map(i=>inv_1i, Q=>inv_0i);
    INV1 : inv port map(i=>inv_0i, Q=>inv_1i);
    
    Q <= inv_1i;
    nQ <= inv_0i;
end Structural;
